/**
 *  @file  EventAware.h
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    12/21/2012
 *
 *  @internal
 *     Created :  12/21/2012
 * Last update :  12/21/2012 03:04:53 PM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */

#ifndef EVENTAWARE_H
#define EVENTAWARE_H


namespace AnHiMa
{

    template<class T> class EventAware
    {
        public:
            EventAware(){};
            ~EventAware(){};
        protected:
            T& event(){return m_event;}
        private:
            T m_event;
    };
}


#endif
