#include "backend/looped/looped.hpp"
#include "natives.hpp"
#include "util/entity.hpp"
#include "util/scripts.hpp"
#include "script_function.hpp"

namespace big
{
	bool bLastDanceMode = false;
	void looped::self_dance_mode()
	{
		if (g->self.dance_mode && SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(RAGE_JOAAT("maintransition")) > 0)
			g->self.dance_mode = false;

		if (g->self.dance_mode && g->self.dance_mode != bLastDanceMode)
		{
			scripts::request_script(RAGE_JOAAT("am_mp_nightclub"));
			if (!scripts::wait_till_loaded(RAGE_JOAAT("am_mp_nightclub")))
				return;

			auto thread = SYSTEM::START_NEW_SCRIPT_WITH_NAME_HASH(RAGE_JOAAT("am_mp_nightclub"), 19400 /*PROPERTY_INT*/);
			SCRIPT::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED(RAGE_JOAAT("am_mp_nightclub"));

			if (!thread)
				return;

			g->m_dance_thread = gta_util::find_script_thread(RAGE_JOAAT("am_mp_nightclub"));
			g->m_dance_program = gta_util::find_script_program(RAGE_JOAAT("am_mp_nightclub"));

			(*g_pointers->m_script_handler_mgr)->attach_thread(g->m_dance_thread);

			g->m_dance_thread->m_context.m_state = rage::eThreadState::unk_3;

			// perform initial setup
			gta_util::execute_as_script(RAGE_JOAAT("am_mp_nightclub"), []
			{
				NETWORK::NETWORK_SET_THIS_SCRIPT_IS_NETWORK_SCRIPT(32, false, 32);
				scr_functions::init_nightclub_script({});
			});

			scr_functions::dance_loop.populate_ip();
			bLastDanceMode = true;
			return;
		}

		if (!g->self.dance_mode && g->self.dance_mode != bLastDanceMode)
		{
			if (g->m_dance_thread)
				g->m_dance_thread->kill();

			g->m_dance_thread = nullptr;
			g->m_dance_program = nullptr;

			bLastDanceMode = false;
			return;
		}

		if (g->self.dance_mode && g->m_dance_thread->m_handler)
		{
			*script_global(1946244).as<bool*>() = true;
			misc::set_bit(scr_globals::gpbd_fm_1.at(self::id, scr_globals::size::gpbd_fm_1).at(267).at(295).at(7).as<int*>(), 0);
			misc::set_bit(scr_globals::gpbd_fm_1.at(self::id, scr_globals::size::gpbd_fm_1).at(267).at(295).at(7).as<int*>(), 1);
			misc::set_bit(scr_globals::gpbd_fm_1.at(self::id, scr_globals::size::gpbd_fm_1).at(267).at(295).at(7).as<int*>(), 2);
			scr_functions::dance_loop.call(g->m_dance_thread, g->m_dance_program, {});
		}
	}
}