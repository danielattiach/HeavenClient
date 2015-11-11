/////////////////////////////////////////////////////////////////////////////
// This file is part of the Journey MMORPG client                           //
// Copyright � 2015 Daniel Allendorf                                        //
//                                                                          //
// This program is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU Affero General Public License as           //
// published by the Free Software Foundation, either version 3 of the       //
// License, or (at your option) any later version.                          //
//                                                                          //
// This program is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of           //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
// GNU Affero General Public License for more details.                      //
//                                                                          //
// You should have received a copy of the GNU Affero General Public License //
// along with this program.  If not, see <http://www.gnu.org/licenses/>.    //
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "IO\UIElement.h"
#include "IO\Components\Textfield.h"
#include "IO\UI.h"
#include "Graphics\Texture.h"
#include "Graphics\Textlabel.h"
#include "Character\Look\CharLook.h"
#include "Util\Randomizer.h"
#include "Net\Session.h"
#include "Net\Login\Login.h"
#include <map>

namespace IO
{
	using::std::map;
	using::std::string;
	using::Util::Randomizer;
	using::Graphics::Texture;
	using::Graphics::Textlabel;
	using::Character::CharLook;
	using::Net::Session;
	using::Net::Login;

	// Character creation screen.
	class UICharcreation : public UIElement
	{
	public:
		enum Buttons
		{
			BT_CHARC_OK,
			BT_CHARC_CANCEL,
			BT_CHARC_FACEL,
			BT_CHARC_FACER,
			BT_CHARC_HAIRL,
			BT_CHARC_HAIRR,
			BT_CHARC_HAIRCL,
			BT_CHARC_HAIRCR,
			BT_CHARC_SKINL,
			BT_CHARC_SKINR,
			BT_CHARC_TOPL,
			BT_CHARC_TOPR,
			BT_CHARC_BOTL,
			BT_CHARC_BOTR,
			BT_CHARC_SHOESL,
			BT_CHARC_SHOESR,
			BT_CHARC_WEPL,
			BT_CHARC_WEPR,
			BT_CHARC_GENDERL,
			BT_CHARC_GEMDERR,
		};

		UICharcreation(Session&, UI&);
		void draw(float) const;
		void update();
		void buttonpressed(uint16_t);
		Mousestate sendmouse(bool, vector2d<int32_t>);
		void nameresult(bool);

	private:
		void randomizelook();

		Session& session;
		UI& ui;

		Texture sky;
		Texture cloud;
		float cloudfx;
		Texture nameboard;
		Textfield namechar;
		vector<Sprite> lookboard;
		CharLook newchar;
		Randomizer randomizer;

		map<bool, vector<uint8_t>> skins;
		map<bool, vector<uint8_t>> haircolors;
		map<bool, vector<int32_t>> faces;
		map<bool, vector<int32_t>> hairs;
		map<bool, vector<int32_t>> tops;
		map<bool, vector<int32_t>> bots;
		map<bool, vector<int32_t>> shoes;
		map<bool, vector<int32_t>> weapons;

		bool named;
		bool female;
		size_t skin;
		size_t haircolor;
		size_t face;
		size_t hair;
		size_t top;
		size_t bot;
		size_t shoe;
		size_t weapon;
		Textlabel facename;
		Textlabel hairname;
		Textlabel haircname;
		Textlabel bodyname;
		Textlabel topname;
		Textlabel botname;
		Textlabel shoename;
		Textlabel wepname;
		Textlabel gendername;
	};

	// Factory for the character creation screen.
	class ElementCharcreation : public Element
	{
	public:
		ElementCharcreation(Session& ses, UI& u) : session(ses), ui(u) {}

		UIType type() const
		{
			return UI_CHARCREATION;
		}

		UIElement* instantiate() const
		{
			return new UICharcreation(session, ui);
		}

	private:
		Session& session;
		UI& ui;
	};
}

