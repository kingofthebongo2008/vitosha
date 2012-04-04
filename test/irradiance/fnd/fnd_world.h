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

		void update_pass_one(float dt)
		{
			on_update_pass_one(dt);
		}

		void update_pass_two(float dt)
		{
			on_update_pass_two(dt);
		}

		private:

		virtual void on_update_pass_one(float dt)
		{
			dt;
		}

		virtual void on_update_pass_two(float dt)
		{
			dt;
		}
	};
}

#endif
