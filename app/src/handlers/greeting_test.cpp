#include <handlers/greeting.hpp>

#include <userver/utest/utest.hpp>

using app::UserType;

UTEST(SayHelloTo, Basic) {
  EXPECT_EQ(app::SayHelloTo("Developer", UserType::kFirstTime),
            "Hello, Developer!\n");
  EXPECT_EQ(app::SayHelloTo({}, UserType::kFirstTime),
            "Hello, unknown user!\n");

  EXPECT_EQ(app::SayHelloTo("Developer", UserType::kKnown),
            "Hi again, Developer!\n");
  EXPECT_EQ(app::SayHelloTo({}, UserType::kKnown), "Hi again, unknown user!\n");
}