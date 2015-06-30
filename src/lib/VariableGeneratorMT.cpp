#include "../crave/backend/VariableGeneratorMT.hpp"
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

namespace crave {
VariableGeneratorMT::VariableGeneratorMT(VariableContainer* vcon) : VariableGenerator(vcon){
}


VariableGeneratorMT::~VariableGeneratorMT() {
}

void VariableGeneratorMT::createNewSolver(ConstraintPartition & partition)
{
    VarSolverPtr vs(new VariableDefaultSolver(var_ctn_, partition));
    boost::mutex::scoped_lock scoped_lock(this->solvers_mutex);
    solvers_.push_back(vs);
}


void VariableGeneratorMT::reset(std::vector<ConstraintPartition>& partitions) {
  boost::mutex::scoped_lock scoped_lock(this->solvers_mutex);
  solvers_.clear();
  scoped_lock.unlock();
  
  std::vector<boost::shared_ptr<boost::thread> > threads;
  BOOST_FOREACH(ConstraintPartition & partition, partitions) { 
    boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&VariableGeneratorMT::createNewSolver,this, boost::ref(partition))));   
    threads.push_back(thread);
  }
  BOOST_FOREACH(boost::shared_ptr<boost::thread> thread, threads)
  {
      thread->join();
  }
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
  std::vector<boost::shared_ptr<boost::thread> > threads;
  BOOST_FOREACH(VarSolverPtr vs, this->solvers_) {
    boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&VariableGeneratorMT::solve,this, vs)));
    threads.push_back(thread);
  }
  BOOST_FOREACH(boost::shared_ptr<boost::thread> thread, threads)
  {
      thread->join();
  }
  return this->solveReturn;
}

}
