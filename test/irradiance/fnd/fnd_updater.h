#ifndef __FND_UPDATER_H__
#define __FND_UPDATER_H__

namespace fnd
{
	class updater
	{
		public:

		virtual ~updater()
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

