///////////////////////////////////////////////////////////////
// ChronoTimer.cpp - uses Chrono::high_resolution_clock      //
//                                                           //
// Platfrom:                                                 //
// Application:  Project #2 help,                            //
//               CSE687 Object Oriented Design, Spring 2014  //
// Author:       Jim Fawcett, CST 4-187, Syracuse University //
///////////////////////////////////////////////////////////////

#include "ChronoTimer.h"
#include <iostream>
#include <thread>

#ifdef TEST_CHRONOTIMER
int main()
{
  std::cout << "\n  Demonstrating ChronoTimer";
  std::cout << "\n ===========================";

  std::cout << "\n  Timing duration of std::this_thread::sleep_for(std::chrono::microseconds(3500))";
  Timer timer;
  for (size_t i = 0; i < 10; ++i)
  {
    timer.start();
    std::this_thread::sleep_for(std::chrono::microseconds(3500));
    timer.stop();
    std::cout << "\n  measured time in nanoseconds = " << timer.elapsedTime<std::chrono::nanoseconds>();
  }
  std::cout << "\n\n";
}
#endif