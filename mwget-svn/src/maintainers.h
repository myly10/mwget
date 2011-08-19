/*  MWget - A Multi download for all POSIX systems.
 *  Homepage: http://mwget.sf.net
 *  Copyright (C) 2005- rgwan,xiaosuo
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

const char maintainer_list[2][3][32]= {
		{"rgwan(WanZhiYuan)","<sa@kmlinux.tk>","http://www.kmlinux.tk"},
		{"xiaosuo(unknown)","<xiao_suo@hotmail.com>",""}				
};

	//鄙视下C++程序，TMD竟然不支持变长！只好每项32字节了……
	//格式是名称，邮件，网站（如果有，没有就NO_WEBSITE）
char count_maintainer=2;//上面的列表数，别-1.

/*		
void init_data_maintainer(void)
{
//---rgwan start

//maintainer_list[0]="<rgwan(WanZhiYuan)>";
//maintainer_list[0][1]="<sa@kmlinux.tk>";

//---rgwan end
//---xiaosuo start
//maintainer_list[1][0]="<xiaosuo(unknown)>";
//maintainer_list[1][1]="<xiao_suo@hotmail.com>";
//---xiaosuo end
}
*/
