#include "../crave/backend/VariableGeneratorMT.hpp"
#include "../crave/backend/VariableDefaultSolver.hpp"
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

namespace crave {
VariableGeneratorMT::VariableGeneratorMT(VariableContainer const& vcon) : VariableGenerator(vcon){
}


VariableGeneratorMT::~VariableGeneratorMT() {
}

void VariableGeneratorMT::createNewSolver(ConstraintPartition & partition,unsigned int index)
{
    //VarSolverPtr vs(new VariableDefaultSolver(var_ctn_, partition));
    var_solv_array[index] = boost::make_shared<VariableDefaultSolver>(var_ctn_,partition);
}


void VariableGeneratorMT::reset(std::vector<ConstraintPartition>& partitions) {
  var_solv_array = new VarSolverPtr[partitions.size()];
  
  boost::thread_group threads;
      for(int i=0;i<partitions.size();i++)
      {
          boost::thread *thread(new boost::thread(boost::bind(&VariableGeneratorMT::createNewSolver,this, boost::ref(partitions.at(i)),i)));  
          threads.add_thread(thread);
      }
  threads.join_all();
  solvers_.clear();
  for(int i=0;i<partitions.size();i++)
  {
      solvers_.push_back(var_solv_array[i]);
  }
  delete []var_solv_array;
}

void VariableGeneratorMT::solve(VarSolverPtr vs)
{
    if(solveReturn)
    {
        bool result = vs->solve();
        boost::mutex::scoped_lock scoped_lock(this->result_mutex);
        this->solveReturn &= result;
    }
}

bool VariableGeneratorMT::solve() {
  this->solveReturn = true; 
  boost::thread_group threads;
  BOOST_FOREACH(VarSolverPtr vs, this->solvers_) {
    boost::thread *thread(new boost::thread(boost::bind(&VariableGeneratorMT::solve,this, vs)));
    threads.add_thread(thread);
  }
  threads.join_all();
  return this->solveReturn;
}

}
