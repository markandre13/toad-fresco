#include "help.hh"
#include "toad/toad.hh"
#include "html/htmlview.hh"
#include "html/urlstream.hh"

#include "html/state.hh"
#include "MainWindow.hh"

int main(int argc, char** argv)
{
	static const char* const about_page =
		"<HTML><BODY BGCOLOR=#0080FF>"
		"<TABLE WIDTH=100% BORDER=1><TR><TD BGCOLOR=#FFE0E0>"
		"<H1>The Fresco WebBrowser</H1></TABLE>"
		"<TABLE WIDTH=100% BORDER=1>"
			"<TR>"
				"<TD BGCOLOR=#FFFFFF>"
				"<TD BGCOLOR=#F0FFFF>"
				"<TD BGCOLOR=#E0F0FF>"
				"<TD BGCOLOR=#D0E0FF>"
				"<TD BGCOLOR=#C0D0FF>"
				"<TD BGCOLOR=#B0C0FF>"
				"<TD BGCOLOR=#A0B0FF>"
				"<TD BGCOLOR=#90A0FF>"
				"<TD BGCOLOR=#8090FF>"
				"<TD BGCOLOR=#7080FF>"
				"<TD BGCOLOR=#6070FF>"
				"<TD BGCOLOR=#5060FF>"
				"<TD BGCOLOR=#4050FF>"
				"<TD BGCOLOR=#3040FF>"
				"<TD BGCOLOR=#2030FF>"
				"<TD BGCOLOR=#1020FF>"
				"<TD BGCOLOR=#0010FF>"
				"<TD BGCOLOR=#0000FF>"
				"<TD BGCOLOR=#0000F0>"
				"<TD BGCOLOR=#0000E0>"
				"<TD BGCOLOR=#0000D0>"
				"<TD BGCOLOR=#0000C0>"
				"<TD BGCOLOR=#0000B0>"
				"<TD BGCOLOR=#0000A0>"
				"<TD BGCOLOR=#000090>"
				"<TD BGCOLOR=#000080>"
				"<TD BGCOLOR=#000070>"
				"<TD BGCOLOR=#000060>"
				"<TD BGCOLOR=#000050>"
				"<TD BGCOLOR=#000040>"
				"<TD BGCOLOR=#000030>"
				"<TD BGCOLOR=#000020>"
				"<TD BGCOLOR=#000010>"
		"</TABLE>"
		"<TABLE WIDTH=100% BORDER=1><TR><TD BGCOLOR=#FFE0E0>"		
		"The Fresco WebBrowser was written by:"
		"<TABLE WIDTH=100%>"
		"<TR>"
			"<TD><H3>Mark-André Hopf</H3>"
			"<TD>"
				"<UL>"
					"<LI>Chief Programmer"
					"<LI>HTML Parser and Viewer Development"
					"<LI>TOAD/Fresco Wrapper Classes"
				"</UL>"
		"<TR>"
			"<TD><H3>Gita Siegert<H3>"
			"<TD>"
				"<UL>"
					"<LI>User Interface"
					"<LI>Icons"
		"</TABLE>"
		"</TABLE>"

		"<TABLE WIDTH=100% BORDER=1>"
			"<TR>"
				"<TD BGCOLOR=#FFFFFF>"
				"<TD BGCOLOR=#F0FFFF>"
				"<TD BGCOLOR=#E0F0FF>"
				"<TD BGCOLOR=#D0E0FF>"
				"<TD BGCOLOR=#C0D0FF>"
				"<TD BGCOLOR=#B0C0FF>"
				"<TD BGCOLOR=#A0B0FF>"
				"<TD BGCOLOR=#90A0FF>"
				"<TD BGCOLOR=#8090FF>"
				"<TD BGCOLOR=#7080FF>"
				"<TD BGCOLOR=#6070FF>"
				"<TD BGCOLOR=#5060FF>"
				"<TD BGCOLOR=#4050FF>"
				"<TD BGCOLOR=#3040FF>"
				"<TD BGCOLOR=#2030FF>"
				"<TD BGCOLOR=#1020FF>"
				"<TD BGCOLOR=#0010FF>"
				"<TD BGCOLOR=#0000FF>"
				"<TD BGCOLOR=#0000F0>"
				"<TD BGCOLOR=#0000E0>"
				"<TD BGCOLOR=#0000D0>"
				"<TD BGCOLOR=#0000C0>"
				"<TD BGCOLOR=#0000B0>"
				"<TD BGCOLOR=#0000A0>"
				"<TD BGCOLOR=#000090>"
				"<TD BGCOLOR=#000080>"
				"<TD BGCOLOR=#000070>"
				"<TD BGCOLOR=#000060>"
				"<TD BGCOLOR=#000050>"
				"<TD BGCOLOR=#000040>"
				"<TD BGCOLOR=#000030>"
				"<TD BGCOLOR=#000020>"
				"<TD BGCOLOR=#000010>"
		"</TABLE>"
		"</BODY></HTML>";

	urlstreambase::save_memory_file("about.html", about_page, strlen(about_page));



	argv[0]="FrescoBrowser";
	InitFresco("Test", argc, argv);
	
	TState::font_normal = new TFont(TFont::SERIF, TFont::PLAIN, 12);
	TState::font_h1     = new TFont(TFont::SANS, TFont::BOLD, 18);
	TState::font_h2     = new TFont(TFont::SANS, TFont::BOLD, 14);
	TState::font_h3     = new TFont(TFont::SANS, TFont::BOLD, 12);
	TState::font_tt     = new TFont(TFont::TYPEWRITER, TFont::PLAIN, 12);

	RunViewer(new TMainWindow());

	delete TState::font_normal;
	delete TState::font_h1;
	delete TState::font_h2;
	delete TState::font_h3;
	delete TState::font_tt;

	return 0;
}
