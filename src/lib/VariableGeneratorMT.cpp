#include "../crave/backend/VariableGeneratorMT.hpp"
#include "../crave/backend/VariableDefaultSolver.hpp"
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

namespace crave {
VariableGeneratorMT::VariableGeneratorMT(VariableContainer const& vcon) : VariableGenerator(vcon){
}

void VariableGeneratorMT::createNewSolver(ConstraintPartition & partition,unsigned int index)
{
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

void VariableGeneratorMT::solve(VarSolverPtr vs, unsigned int index)
{
    result_array[index] = vs->solve();
}

bool VariableGeneratorMT::solve() {
  result_array = new bool[this->solvers_.size()];
  boost::thread_group threads;
  for(unsigned int i=0;i<this->solvers_.size();i++)
  {
    boost::thread *thread(new boost::thread(boost::bind(&VariableGeneratorMT::solve,this, this->solvers_.at(i),i)));
    threads.add_thread(thread);
  }
  threads.join_all();
  bool result = true;
  for(unsigned int i=0;i<this->solvers_.size();i++)
  {
      result &= result_array[i];
  }
  delete [] result_array;
  return result;
}

}
