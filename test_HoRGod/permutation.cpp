#define BOOST_TEST_MODULE offline_online
#include <emp-tool/emp-tool.h>
#include <io/netmp.h>
#include <HoRGod/offline_evaluator.h>
#include <HoRGod/online_evaluator.h>
#include <HoRGod/sharing.h>
#include <HoRGod/types.h>

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>
#include <cmath>
#include <future>
#include <memory>
#include <string>
#include <vector>

using namespace HoRGod;
using namespace HoRGod::utils;
namespace bdata = boost::unit_test::data;

constexpr int TEST_DATA_MAX_VAL = 1000;
constexpr int SECURITY_PARAM = 128;
constexpr int seed = 200;
constexpr int cm_threads = 1;

BOOST_AUTO_TEST_SUITE(offline_online_evaluator)
BOOST_AUTO_TEST_CASE(permu_gate) {
  auto seed = emp::makeBlock(100, 200);
  int nf = 10;
  Circuit<Ring> circ;
  std::vector<wire_t> vwa(nf);
  std::vector<wire_t> vwb(nf);
  for (int i = 0; i < nf; i++) {
    vwa[i] = circ.newInputWire();
    vwb[i] = circ.newInputWire();
  }
  auto wdotp = circ.addGate_permu(GateType::kPerm, vwa, vwb);
  circ.setAsOutput(wdotp);
  auto level_circ = circ.orderGatesByLevel();

  std::unordered_map<wire_t, Ring> input_map;
  std::unordered_map<wire_t, int> input_pid_map;
  std::mt19937 gen(200);
  std::uniform_int_distribution<Ring> distrib(0, TEST_DATA_MAX_VAL);
  for (size_t i = 0; i < nf; ++i) {
    input_map[vwa[i]] = i;
    input_map[vwb[i]] = nf-1-i;
    input_pid_map[vwa[i]] = 0;
    input_pid_map[vwb[i]] = 1;
  }

  auto exp_output = circ.evaluate(input_map);

//   std::vector<std::future<std::vector<Ring>>> parties;
//   for (int i = 0; i < 5; ++i) {
//     parties.push_back(std::async(std::launch::async, [&, i]() {
//       auto network_offline = std::make_shared<io::NetIOMP<5>>(i, 10002, nullptr, true);
//       auto network = std::make_shared<io::NetIOMP<5>>(i, 10000, nullptr, true);
//       emp::PRG prg(&seed, 0);
//       OfflineEvaluator offline_eval(i, std::move(network_offline), nullptr, level_circ, SECURITY_PARAM, cm_threads);
//       // auto preproc = 
//       auto preproc = offline_eval.offline_setwire(level_circ, input_pid_map, SECURITY_PARAM, i, prg); //每个i需要预处理

//       // OfflineEvaluator::dummy(level_circ, input_pid_map,
//       //                                        SECURITY_PARAM, i, prg);
//       OnlineEvaluator online_eval(i, std::move(network), std::move(preproc),
//                                   level_circ, SECURITY_PARAM, 1);

//       return online_eval.evaluateCircuit(input_map);
//     }));
//   }

    // std::cout<<"output: ";
    // for (const auto& num : output) {
    //     std::cout << num << " ";
    // }
    std::cout<<"exp_output: ";
    for (const auto& num : exp_output) {
        std::cout << num << " ";
    }
    std::cout<<endl;
//   for (auto& p : parties) {
//     auto output = p.get();
//     BOOST_TEST(output == exp_output);
//   }
}
BOOST_AUTO_TEST_SUITE_END()