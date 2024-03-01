#pragma once
#include <string>

using namespace std;

namespace States
{
    class IState
    {
        public:
        /// @brief Gets the name of the state
        /// @return 
        virtual string GetName() = 0;
        /// @brief Performs the action of the state
        virtual void DoAction() = 0;
        /// @brief Fires after the action caused a wait
        virtual void WaitDone() = 0;
    };
}
