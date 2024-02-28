#include "IState.h"

namespace States
{
    class Context
    {
        private:
        IState* _state;

        public:
        Context():_state(nullptr)
        {
            
        }
    };
}