/* secondary.{cc,hh} -- code for generating fake glyphs
 *
 * Copyright (c) 2003 Eddie Kohler
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version. This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 */

#include <config.h>
#include "secondary.hh"
#include "metrics.hh"
#include "dvipsencoding.hh"
#include <efont/t1bounds.hh>
#include <stdarg.h>

enum { U_CWM = 0x200C,		// U+200C ZERO WIDTH NON-JOINER
       U_VISIBLESPACE = 0x2423,	// U+2423 OPEN BOX
       U_SS = 0xD800,		// invalid Unicode
       U_EMPTYSLOT = 0xD801,	// invalid Unicode (not handled by Secondary)
       U_ALTSELECTOR = 0xD802,	// invalid Unicode
       U_VS1 = 0xFE00,
       U_VS16 = 0xFE0F,
       U_VS17 = 0xE0100,
       U_VS256 = 0xE01FF,
       U_IJ = 0x0132,
       U_ij = 0x0133,
       U_DOTLESSJ = 0x0237,
       U_DOTLESSJ_2 = 0xF6BE };

Secondary::~Secondary()
{
}

bool
Secondary::encode_uni(int code, PermString name, uint32_t uni, const DvipsEncoding &dvipsenc, Metrics &m)
{
    Vector<Setting> v;
    if (setting(uni, v, dvipsenc)) {
	m.encode_virtual(code, name, v);
	return true;
    } else if (_next)
	return _next->encode_uni(code, name, uni, dvipsenc, m);
    else
	return false;
}

T1Secondary::T1Secondary(const Efont::Cff::Font *cff, const Efont::OpenType::Cmap &cmap)
    : _xheight(1000), _spacewidth(1000)
{
    int bounds[4], width;
    
    static const int xheight_unis[] = { 'x', 'm', 'z', 0 };
    for (const int *x = xheight_unis; *x; x++)
	if (char_bounds(bounds, width, cff, cmap, *x, Transform()) && bounds[3] < _xheight)
	    _xheight = bounds[3];

    if (char_bounds(bounds, width, cff, cmap, ' ', Transform()))
	_spacewidth = width;
}

bool
Secondary::setting(uint32_t uni, Vector<Setting> &v, const DvipsEncoding &dvipsenc)
{
    if (_next)
	return _next->setting(uni, v, dvipsenc);
    else
	return false;
}

bool
T1Secondary::two_char_setting(uint32_t uni1, uint32_t uni2, Vector<Setting> &v, const DvipsEncoding &dvipsenc)
{
    int c1 = dvipsenc.encoding_of_unicode(uni1);
    int c2 = dvipsenc.encoding_of_unicode(uni2);
    if (c1 >= 0 && c2 >= 0) {
	v.push_back(Setting(Setting::SHOW, c1));
	v.push_back(Setting(Setting::KERN));
	v.push_back(Setting(Setting::SHOW, c2));
	return true;
    } else
	return false;
}

bool
T1Secondary::encode_uni(int code, PermString name, uint32_t uni, const DvipsEncoding &dvipsenc, Metrics &m)
{
    if (uni == U_ALTSELECTOR || (uni >= U_VS1 && uni <= U_VS16) || (uni >= U_VS17 && uni <= U_VS256)) {
	Vector<Setting> v;
	setting(uni, v, dvipsenc);
	int which = (uni == U_ALTSELECTOR ? 0 : (uni <= U_VS16 ? uni - U_VS1 + 1 : uni - U_VS17 + 17));
	m.encode_virtual(code, (which ? permprintf("<vs%d>", which) : PermString("<altselector>")), v);
	m.add_altselector_code(code, which);
	return true;
    } else
	return Secondary::encode_uni(code, name, uni, dvipsenc, m);
}

bool
T1Secondary::setting(uint32_t uni, Vector<Setting> &v, const DvipsEncoding &dvipsenc)
{
    switch (uni) {
	
      case U_CWM:
      case U_ALTSELECTOR:
	v.push_back(Setting(Setting::RULE, 0, _xheight));
	return true;

      case U_VISIBLESPACE:
	v.push_back(Setting(Setting::MOVE, 50, -150));
	v.push_back(Setting(Setting::RULE, 40, 150));
	v.push_back(Setting(Setting::RULE, _spacewidth, 40));
	v.push_back(Setting(Setting::RULE, 40, 150));
	v.push_back(Setting(Setting::MOVE, 50, 150));
	return true;

      case U_SS:
	if (two_char_setting('S', 'S', v, dvipsenc))
	    return true;
	break;

      case U_IJ:
	if (two_char_setting('I', 'J', v, dvipsenc))
	    return true;
	break;

      case U_ij:
	if (two_char_setting('i', 'j', v, dvipsenc))
	    return true;
	break;

      case U_DOTLESSJ:
      case U_DOTLESSJ_2:
	v.push_back(Setting(Setting::FONT, 1));
	v.push_back(Setting(Setting::SHOW, 'j'));
	break;
	
    }

    // variant selectors get the same setting as VSCHOICE
    if ((uni >= U_VS1 && uni <= U_VS16) || (uni >= U_VS17 && uni <= U_VS256))
	return setting(U_ALTSELECTOR, v, dvipsenc);

    // otherwise, try other secondaries
    return Secondary::setting(uni, v, dvipsenc);
}


bool
char_bounds(int bounds[4], int &width,
	    const Efont::Cff::Font *cff, const Efont::OpenType::Cmap &cmap,
	    uint32_t uni, const Transform &transform)
{
    if (Efont::OpenType::Glyph g = cmap.map_uni(uni))
	return Efont::CharstringBounds::bounds(transform, cff->glyph_context(g), bounds, width);
    else
	return false;
}
