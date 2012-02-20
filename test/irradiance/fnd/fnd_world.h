#ifndef __FND_WORLD_H__
#define __FND_WORLD_H__

namespace fnd
{
	class world
	{
		public:

		virtual ~world()
		{

		}

		void update(float dt)
		{
			on_update(dt);
		}

		private:

		virtual void on_update(float dt)
		{
			dt;
		}
	};
}

#endif
