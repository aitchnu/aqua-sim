#ifndef _SCREEN_HPP_
#define _SCREEN_HPP_

using namespace std;

class Screen
{
	public:
		virtual ~Screen()
		{
		}
		;
		virtual void load()
		{
		}
		;
		virtual void pause()
		{
		}
		;
		virtual void update()
		{
		}
		;
		virtual void render()
		{
		}
		;
};

#endif
