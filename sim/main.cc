#include "header.h"
using namespace std;

Statistics stat;
Config cfg;

int main(int argc, char const *argv[]) {
  Simulator sim;
  string wlpath;
  string cfgpath;
  if (argc >=2) {
    string in(argv[1]);
    wlpath = in;
  }
  else {
    wlpath = "../workloads/test/";
  }
  if(argc >= 3) {
    string cfgname(argv[2]);
    cfgpath = "../sim/config/" + cfgname;
  }
  else 
    cfgpath = "../sim/config/default.txt";

  string gname = wlpath + "/output/graphOutput.txt";
  string mname = wlpath + "/output/mem.txt";
  string cname = wlpath + "/output/ctrl.txt";
  
  cfg.vInputLevel = -1;

  // enable verbosity level: -v
  if (argc == 4) {
    string v(argv[3]);
    if (v == "-v")
       cfg.vInputLevel = 1;
  }

  Reader r; 
  r.readCfg(cfgpath);
  r.readGraph(gname, sim.g);
  r.readProfMemory(mname , sim.memory);
  r.readProfCF(cname, sim.cf);
  
  GraphOpt opt(sim.g);
  opt.inductionOptimization();
  cout << "[5] Initialization Complete \n";
  sim.initialize();
  sim.run();
  return 0;
} 