#define BOOST_TEST_MODULE jump
#include <emp-tool/emp-tool.h>
#include <io/netmp.h>
#include <HoRGod/ijmp.h>

#include <boost/test/included/unit_test.hpp>
#include <future>
#include <string>
#include <vector>

using namespace HoRGod;

BOOST_AUTO_TEST_SUITE(ijump_provider)

BOOST_AUTO_TEST_CASE(all_combinations) {
  std::string message("A test string.");
  std::vector<uint8_t> input(message.begin(), message.end());
  
  std::vector<std::future<void>> parties;
  for (int i = 0; i < 5; ++i) {
    parties.push_back(std::async(std::launch::async, [&, i]() {
    // parties.push_back(tpool.enqueue([&]() {
      io::NetIOMP<5> network(i, 10000, nullptr, true);
      ImprovedJmp jump(i);
      ThreadPool tpool(1);

      for (int sender1 = 0; sender1 < 5; ++sender1) {
        for (int sender2 = sender1 + 1; sender2 < 5; ++sender2) {
          for (int sender3 = sender2 + 1; sender3 < 5; ++sender3) {
            for (int receiver = 0; receiver < 5; ++receiver) {
              if (receiver == sender1 || receiver == sender2) {
                continue;
              }
              jump.jumpUpdate(sender1, sender2, sender3, receiver, input.size(),
                              input.data());
            }
          }
          
        }
      }

      jump.communicate(network, tpool);

      for (int sender1 = 0; sender1 < 5; ++sender1) {
        for (int sender2 = sender1 + 1; sender2 < 5; ++sender2) {
          for (int sender3 = sender2 + 1; sender3 < 5; ++sender3) {
            if (i == sender1 || i == sender2 || i == sender3) {
              continue;
            }
            BOOST_TEST(jump.getValues(sender1, sender2, sender3) == input);
          }
        }
      }
    }));
  }

  for (auto& p : parties) {
    p.wait();
  }
}

BOOST_AUTO_TEST_SUITE_END()
