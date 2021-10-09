//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.											//
//																				//
//	This program is distributed in the hope that it will be useful,				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero General Public License for more details.							//
//																				//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#include "UILogin.h"

#include "UILoginNotice.h"
#include "UILoginWait.h"

#include "../UI.h"

#include "../Components/MapleButton.h"

#include "../../Audio/Audio.h"

#include "../../Net/Packets/LoginPackets.h"

#include <windows.h>

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

namespace ms
{
	UILogin::UILogin() : UIElement(Point<int16_t>(0, 0), Point<int16_t>(800, 600)), signboard_pos(Point<int16_t>(510, 333))
	{
		LoginStartPacket().dispatch();

		std::string LoginMusicNewtro = Configuration::get().get_login_music();

		Music(LoginMusicNewtro).play();

		std::string version_text = Configuration::get().get_version();
		version = Text(Text::Font::A11B, Text::Alignment::LEFT, Color::Name::LEMONGRASS, "Ver. " + version_text);

		nl::node map = nl::nx::map["Back"]["login.img"];
		nl::node back = map["back"];
		nl::node ani = map["ani"];

		nl::node Login = nl::nx::ui["Login.img"];
		nl::node Title = Login["Title"];
		nl::node Common = Login["Common"];

		nl::node MapTitle = nl::nx::map["Obj"]["login.img"]["Title"];
		nl::node logo = MapTitle["logo"]["0"]["0"];
		nl::node effect = MapTitle["effect"];
		nl::node frame = nl::nx::ui["Obj"]["Login.img"]["Common"]["frame"];

		sprites.emplace_back(back["11"], Point<int16_t>(400, 300));
		sprites.emplace_back(effect["0"], Point<int16_t>(580, 20));
		sprites.emplace_back(effect["1"], Point<int16_t>(580, 25));
		sprites.emplace_back(effect["2"], Point<int16_t>(580, 27));
		sprites.emplace_back(effect["3"], Point<int16_t>(580, 31));
		sprites.emplace_back(logo, Point<int16_t>(409, 144));
		sprites.emplace_back(nl::nx::map["Obj"]["login.img"]["Title"]["signboard"]["0"]["0"], signboard_pos);
		sprites.emplace_back(frame, Point<int16_t>(400, 300));
		sprites.emplace_back(Common["frame"], Point<int16_t>(400, 300));

		buttons[Buttons::BT_LOGIN] = std::make_unique<MapleButton>(Title["BtLogin"], signboard_pos + Point<int16_t>(85, -106));
		buttons[Buttons::BT_SAVEID] = std::make_unique<MapleButton>(Title["BtLoginIDSave"], signboard_pos + Point<int16_t>(-91, -27));
		buttons[Buttons::BT_IDLOST] = std::make_unique<MapleButton>(Title["BtLoginIDLost"], signboard_pos + Point<int16_t>(20, -27));
		buttons[Buttons::BT_PASSLOST] = std::make_unique<MapleButton>(Title["BtPasswdLost"], signboard_pos + Point<int16_t>(100, -27));
		buttons[Buttons::BT_REGISTER] = std::make_unique<MapleButton>(Title["BtNew"], signboard_pos + Point<int16_t>(-113, 20));
		buttons[Buttons::BT_HOMEPAGE] = std::make_unique<MapleButton>(Title["BtHomePage"], signboard_pos + Point<int16_t>(-13, 20));
		buttons[Buttons::BT_QUIT] = std::make_unique<MapleButton>(Title["BtQuit"], signboard_pos + Point<int16_t>(86, 20));

		checkbox[false] = Title["check"]["0"];
		checkbox[true] = Title["check"]["1"];

		background = ColorBox(dimension.x(), dimension.y(), Color::Name::BLACK, 1.0f);

		Point<int16_t> textbox_pos = signboard_pos + Point<int16_t>(-68, -98);
		Point<int16_t> textbox_dim = Point<int16_t>(145, 20);
		int16_t textbox_limit = 12;

#pragma region Account
		account = Textfield(Text::Font::A13M, Text::Alignment::LEFT, Color::Name::WHITE, Rectangle<int16_t>(textbox_pos, textbox_pos + textbox_dim), textbox_limit);

		account.set_key_callback
		(
			KeyAction::Id::TAB, [&]
			{
				account.set_state(Textfield::State::NORMAL);
				password.set_state(Textfield::State::FOCUSED);
			}
		);

		account.set_enter_callback
		(
			[&](std::string msg)
			{
				login();
			}
		);

		accountbg = Title["ID"];
#pragma endregion

#pragma region Password
		textbox_pos.shift_y(30);

		password = Textfield(Text::Font::A13M, Text::Alignment::LEFT, Color::Name::WHITE, Rectangle<int16_t>(textbox_pos, textbox_pos + textbox_dim), textbox_limit);

		password.set_key_callback
		(
			KeyAction::Id::TAB, [&]
			{
				account.set_state(Textfield::State::FOCUSED);
				password.set_state(Textfield::State::NORMAL);
			}
		);

		password.set_enter_callback
		(
			[&](std::string msg)
			{
				login();
			}
		);

		password.set_cryptchar('*');
		passwordbg = Title["PW"];
#pragma endregion

		saveid = Setting<SaveLogin>::get().load();

		if (saveid)
		{
			account.change_text(Setting<DefaultAccount>::get().load());
			password.set_state(Textfield::State::FOCUSED);
		}
		else
		{
			account.set_state(Textfield::State::FOCUSED);
		}

		if (Configuration::get().get_auto_login())
		{
			UI::get().emplace<UILoginWait>([]() {});

			auto loginwait = UI::get().get_element<UILoginWait>();

			if (loginwait && loginwait->is_active())
				LoginPacket(
					Configuration::get().get_auto_acc(),
					Configuration::get().get_auto_pass()
				).dispatch();
		}
	}

	void UILogin::draw(float alpha) const
	{
		background.draw(position + Point<int16_t>(0, 7));

		UIElement::draw(alpha);

		version.draw(position + Point<int16_t>(680, 4));
		account.draw(position + Point<int16_t>(5, 0));
		password.draw(position + Point<int16_t>(5, 3));

		/*
		if (account.get_state() == Textfield::State::NORMAL && account.empty())
			accountbg.draw(position + signboard_pos + Point<int16_t>(-100, -51));

		if (password.get_state() == Textfield::State::NORMAL && password.empty())
			passwordbg.draw(position + signboard_pos + Point<int16_t>(-100, -25));
		*/

		checkbox[saveid].draw(position + signboard_pos + Point<int16_t>(-115, -27));
	}

	void UILogin::update()
	{
		UIElement::update();

		account.update();
		password.update();
	}

	void UILogin::login()
	{
		account.set_state(Textfield::State::DISABLED);
		password.set_state(Textfield::State::DISABLED);

		std::string account_text = account.get_text();
		std::string password_text = password.get_text();

		std::function<void()> okhandler = [&, password_text]()
		{
			account.set_state(Textfield::State::NORMAL);
			password.set_state(Textfield::State::NORMAL);

			if (!password_text.empty())
				password.set_state(Textfield::State::FOCUSED);
			else
				account.set_state(Textfield::State::FOCUSED);
		};

		if (account_text.empty())
		{
			UI::get().emplace<UILoginNotice>(UILoginNotice::Message::NOT_REGISTERED, okhandler);
			return;
		}

		if (password_text.length() <= 4)
		{
			UI::get().emplace<UILoginNotice>(UILoginNotice::Message::WRONG_PASSWORD, okhandler);
			return;
		}

		UI::get().emplace<UILoginWait>(okhandler);

		auto loginwait = UI::get().get_element<UILoginWait>();

		if (loginwait && loginwait->is_active())
			LoginPacket(account_text, password_text).dispatch();
	}

	void UILogin::open_url(uint16_t id)
	{
		std::string url;

		switch (id)
		{
			case Buttons::BT_REGISTER:
				url = Configuration::get().get_joinlink();
				break;
			case Buttons::BT_HOMEPAGE:
				url = Configuration::get().get_website();
				break;
			case Buttons::BT_PASSLOST:
				url = Configuration::get().get_findpass();
				break;
			case Buttons::BT_IDLOST:
				url = Configuration::get().get_findid();
				break;
			default:
				return;
		}

		ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}

	Button::State UILogin::button_pressed(uint16_t id)
	{
		switch (id)
		{
			case Buttons::BT_LOGIN:
			{
				login();

				return Button::State::NORMAL;
			}
			case Buttons::BT_REGISTER:
			case Buttons::BT_HOMEPAGE:
			case Buttons::BT_PASSLOST:
			case Buttons::BT_IDLOST:
			{
				open_url(id);

				return Button::State::NORMAL;
			}
			case Buttons::BT_SAVEID:
			{
				saveid = !saveid;

				Setting<SaveLogin>::get().save(saveid);

				return Button::State::MOUSEOVER;
			}
			case Buttons::BT_QUIT:
			{
				UI::get().quit();

				return Button::State::PRESSED;
			}
			default:
			{
				return Button::State::DISABLED;
			}
		}
	}

	Cursor::State UILogin::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (Cursor::State new_state = account.send_cursor(cursorpos, clicked))
			return new_state;

		if (Cursor::State new_state = password.send_cursor(cursorpos, clicked))
			return new_state;

		return UIElement::send_cursor(clicked, cursorpos);
	}

	UIElement::Type UILogin::get_type() const
	{
		return TYPE;
	}
}