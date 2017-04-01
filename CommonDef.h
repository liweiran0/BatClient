#pragma once
#include <chrono>
#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <WS2tcpip.h>
#include <WinSock2.h>

using namespace std;

typedef function<void(string)> Callback;