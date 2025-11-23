#include "tcp/server.hpp"

int main()
{
    Server s{8081};
    s.run();
    return 0;
}
