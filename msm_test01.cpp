//          Copyright coiled-coil 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// This program is example for Boost.MSM and Boost.Test.
// See also:
//    Boost C++ Library
//    http://www.boost.org/

#include <iostream>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>

#if 1
#   define BOOST_TEST_DYN_LINK
#   define BOOST_TEST_MAIN
#   define BOOST_TEST_MODULE MyTest
#   include <boost/test/unit_test.hpp>
#else
#   define BOOST_TEST_NO_LIB
#   define BOOST_TEST_MAIN
#   define BOOST_TEST_MODULE MyTest
#   include <boost/test/included/unit_test.hpp>
#endif

namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace std;

namespace
{
    // Events
    class start_download
    {
    public:
        explicit start_download(std::string const& url)
        :   url_(url)
        {}
    private:
        std::string url_;
    };
    struct notify_content_ready {};
    struct notify_error {};

    // front-end: define the FSM structure
    struct downloader_
    :   public msm::front::state_machine_def<downloader_>
    {

        // The list of FSM states
        struct Uninitialized : public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const& e, FSM& )
            {
                cout << "[Uninitialized] " << typeid(e).name() << endl;
            }
        };

        struct Downloading : public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const& e, FSM& )
            {
                cout << "[Downloading] " << typeid(e).name() << endl;
            }
        };

        struct ContentReady : public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const& e, FSM& )
            {
                cout << "[ContentReady] " << typeid(e).name() << endl;
            }
        };

        struct Error : public msm::front::state<>
        {
        };

        // the initial state of the player SM. Must be defined
        typedef Uninitialized initial_state;

        // transition actions
        void handle_download(start_download const& event)
        {
        }

        // guard conditions
        // Nothing

        typedef downloader_ x; // makes transition table cleaner

        // Transition table for player
        struct transition_table : mpl::vector<
            //    Start                  Event               Next              Action               Guard
            //  +--------------+---------------------+----------------+---------------------+----------------------+
          a_row < Uninitialized, start_download      , Downloading    , &x::handle_download                        >,
            //  +--------------+---------------------+----------------+---------------------+----------------------+
           _row < Downloading  , notify_content_ready, ContentReady                                                >,
           _row < Downloading  , notify_error        , Error                                                       >,
            //  +--------------+---------------------+----------------+---------------------+----------------------+
          a_row < ContentReady , start_download      , Downloading    , &x::handle_download                        >,
            //  +--------------+---------------------+----------------+---------------------+----------------------+
          a_row < Error        , start_download      , Downloading    , &x::handle_download                        >
            //  +--------------+---------------------+----------------+---------------------+----------------------+
        > {};

        // Replaces the default no-transition response.
        template <class FSM,class Event>
        void no_transition(Event const& , FSM&,int)
        {
            BOOST_FAIL("no_transition called!");
        }

        // init counters
        template <class Event,class FSM>
        void on_entry(Event const&,FSM& fsm) 
        {
        }
    };

    // Pick a back-end
    typedef msm::back::state_machine<downloader_> downloader;

    static char const* const state_names[] = { "Uninitialized", "Downloading", "ContentReady", "Error", };
}

BOOST_AUTO_TEST_CASE( my_test )
{
    downloader p;

    p.start();
    BOOST_CHECK_EQUAL(state_names[p.current_state()[0]], "Uninitialized");

    p.process_event(start_download("http://google.com/"));
    BOOST_CHECK_EQUAL(state_names[p.current_state()[0]], "Downloading");

    p.process_event(notify_content_ready());
    BOOST_CHECK_EQUAL(state_names[p.current_state()[0]], "ContentReady");
}
