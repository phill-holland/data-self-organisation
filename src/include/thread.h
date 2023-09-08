#include <thread>
#include <chrono>
//#include "core/threading/mutex.h"

#ifndef _CORE_THREADING_THREAD
#define _CORE_THREADING_THREAD

namespace core
{
	namespace threading
	{
		class thread
		{
		protected:
			volatile bool running;
			volatile bool paused;
			volatile bool iampaused;
			volatile bool stopped;
			volatile bool neverstarted;
			volatile bool requestedstop;

			static std::chrono::milliseconds milliseconds;

		public:
			static void interrupt(core::threading::thread *bt)
			{
				while(!bt->requestedstop)
				{
					bt->neverstarted = false;
					if (bt->paused)
					{
						bt->iampaused = true; std::this_thread::sleep_for(milliseconds);
					}
					else
					{
						bt->iampaused = false;
						bt->background(bt);
					}
				};

				bt->stopped = true;
				bt->running = false;
			}

			virtual void background(core::threading::thread *bt) = 0;

		private:
			std::thread *handle;

			bool init;

		public:
			thread();
			~thread() { cleanup(); }

			bool start() { init = create(); return init; }
			bool initalised() { return init; }

			void stop();

			void pause(bool s);
			bool isPaused() { return iampaused; }
			bool isRunning() { return running; }

			bool pauseAndWait(bool value, long time, long max);
			bool pauseAndWait(bool value) { return pauseAndWait(value, 500L); }
			bool pauseAndWait(bool value, long time) { return pauseAndWait(value, time, 30L); }

			bool stopAndWait(long time, long max) { stop(); return true; }
			bool stopAndWait(long time) { return stopAndWait(time, 30L); }
			bool stopAndWait() { return stopAndWait(500L); }

			bool WaitLongTimeWithInteruption(long time);

			void sleep(int ms)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(ms));
			}

		private:
			bool create();

		private:
			void makeNull() { handle = NULL; }
			void cleanup();
		};
	};
};

#endif