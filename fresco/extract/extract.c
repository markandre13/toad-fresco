#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *in;
FILE *idx;
FILE *out;
const char *filename;
char module[512];

void parse_head(char*);
char* read_line();

int ungetline;

char* read_line()
{
	static char buffer[2049];
	char *line, *p;
	int len;

	repeat:
	if (!ungetline)
	{
		memset(buffer,0,2049);	
		fgets(buffer,2048,in);
	}
	ungetline=0;
	
	len = strlen(buffer);
	if (len>0)
	{
		if (buffer[len-1]=='\n')
			buffer[len-1]=0;
		line = buffer + strspn(buffer, " \t\r\n");
		if (line[0]=='#' && !feof(in))
			goto repeat;
		if (strncmp(line,"module",6)==0)
		{
			line+=6;
			line+=strspn(line, " \t\r\n");
			
			p = module;
			while(1)
			{
				*p = *line;
				if ( !isalpha(*line) && !isdigit(*line) && *line!='_' )
					break;
				p++;
				line++;
			}
			*(p++)=':';
			*(p++)=':';
			*p=0;
			
			goto repeat;
		}
		return line;
	}
	return buffer;
}

void printbar(const char* color, const char *text)
{
	fprintf(out,"<TABLE WIDTH=100% CELLPADDING=2><TR><TD BGCOLOR=\"%s\">\n",color);
	fprintf(out,"<B><FONT FACE=helvetica>%s</FONT></B>\n",text);
	fprintf(out,"</TD></TR></TABLE>\n");
}

int main(int argc, char **argv)
{
	char *start;
	int i,type,len;

	ungetline = 0;
	idx = fopen("index.html","w");
	fprintf(idx,
		"<HTML><BODY>Index<HR>\n");

	for(i=1; i<argc; i++)
	{
		printf("%s...\n",argv[i]);
		in = fopen(argv[i],"r");
		filename = argv[i];
		module[0]=0;
		while(1)
		{
			start = read_line();
			if (feof(in))
				break;
			if (strncmp(start,"//",2)==0)
			{
				start+=2;
				type = *start;
				start += strspn(start+1, " \t\r\n");
				switch(type)
				{
					case '-':
						parse_head(start);
						break;
					case '.':
						fprintf(out,"%s\n",start);
						break;
				}
			}
		}
		fclose(in);
	}
	fprintf(out,
		"<HR><SMALL><FONT FACE=helvetica>"
		"This reference is an extract from the Fresco 0.1.7 IDL files:<BR>"
		"Copyright &copy; 1992-94 Silicon Graphics,&nbsp;Inc.<BR>"
		"Copyright &copy; 1993-94 Fujitsu,&nbspLtd.<BR>"
		"Copyright &copy; 1995 Sun Microsystems,&nbspInc.<BR>"
		"Copyright &copy; 1997 Mark-André&nbsp;Hopf<BR>"
		"</FONT></BODY></HTML>\n");
	fclose(out);

	fprintf(idx,"</BODY>\n");
	fclose(idx);
}

void parse_head(char *start)
{
	char buffer[2049];
	char htmlfile[256];
	char *line, *p;
	const char *color;
	int i, insert=0;

	memset(buffer,0,2049);
	do {
		line = read_line();
		if (feof(in))
			return;
	}while(strncmp(line,"//-",3)==0);

	if (strncmp(line,"interface",9)==0)
	{
		line+=9;
		line+=strspn(line, " \t\r\n");
		p = buffer;
		while(1)
		{
			*p = *line;
			if ( !isalpha(*line) && !isdigit(*line) && *line!='_' )
				break;
			p++;
			line++;
		}
		*p=0;
		sprintf(htmlfile,"%s%s.html",module,buffer);
		fprintf(idx,"<A HREF=\"%s\">%s%s</A><BR>\n",htmlfile,module,buffer);
		if (out)
		{
			fprintf(out,
				"<HR><SMALL><FONT FACE=helvetica>"
				"This reference is an extract from the Fresco 0.1.7 IDL files:<BR>"
				"Copyright &copy; 1992-94 Silicon Graphics,&nbsp;Inc.<BR>"
				"Copyright &copy; 1993-94 Fujitsu,&nbspLtd.<BR>"
				"Copyright &copy; 1995 Sun Microsystems,&nbspInc.<BR>"
				"Copyright &copy; 1997 Mark-André&nbsp;Hopf<BR>"
				"</FONT></BODY></HTML>\n");
			fclose(out);
		}
		out = fopen(htmlfile,"w");
		fprintf(out,
			"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2//EN\">\n"
			"<HTML><HEAD><TITLE>Fresco 0.1.7 Reference Manual</TITLE></HEAD>"
			"<BODY BGCOLOR=#FFC080>"
			"<TABLE WIDTH=100% CELLPADDING=0 CELLSPACING><TR><TD><B><BIG><FONT FACE=helvetica>"
			"&nbsp;%s%s</FONT></BIG></B>\n"
			"</TD><TD ALIGN=RIGHT><FONT FACE=helvetica>[<A HREF=\"index.html\">index</A>]</FONT></TD></TR></TABLE>\n"
			"<HR><P>\n",
			module,buffer);

		fprintf(out,"<B><FONT FACE=helvetica>declared in file:</FONT></A></B> %s<BR>",filename);

		line+=strspn(line, " \t\r\n");
		if (*line==':')
		{
			line++;
			fprintf(out,"<B><FONT FACE=helvetica>ancestors:</FONT></B> ");
			while(1)
			{
				line+=strspn(line, " \t\r\n");
				if (*line == '{')
					break;
				if (*line == ',')
				{
					fprintf(out, ", ");
					line++;
					continue;
				}
				p = buffer;
				while(1)
				{
					*p = *line;
					if ( !isalpha(*line) && !isdigit(*line) && *line!='_' && *line!=':')
						break;
					p++;
					line++;
				}
				*p=0;
				fprintf(out,"<A HREF=\"%s.html\">%s</A>",buffer,buffer);
			}
		}
		fprintf(out, "<P>");
		return;
	}
	else if (strncmp(line,"class",5)==0)
	{
		line+=5;
		line+=strspn(line, " \t\r\n");
		if (strncmp(line,"_base_dll",9)==0)
		{
			line+=9;
			line+=strspn(line, " \t\r\n");
		}
		p = buffer;
		while(1)
		{
			*p = *line;
			if ( !isalpha(*line) && !isdigit(*line) && *line!='_' )
				break;
			p++;
			line++;
		}
		*p=0;
		sprintf(htmlfile,"%s%s.html",module,buffer);
		fprintf(idx,"<A HREF=\"%s\">%s%s</A><BR>\n",htmlfile,module,buffer);
		if (out)
		{
			fprintf(out,
				"<HR><SMALL><FONT FACE=helvetica>"
				"This reference is an extract from the Fresco 0.1.7 IDL files:<BR>"
				"Copyright &copy; 1992-94 Silicon Graphics,&nbsp;Inc.<BR>"
				"Copyright &copy; 1993-94 Fujitsu,&nbspLtd.<BR>"
				"Copyright &copy; 1995 Sun Microsystems,&nbspInc.<BR>"
				"Copyright &copy; 1997 Mark-André&nbsp;Hopf<BR>"
				"</FONT></BODY></HTML>\n");
			fclose(out);
		}
		out = fopen(htmlfile,"w");
		fprintf(out,
			"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2//EN\">\n"
			"<HTML><HEAD><TITLE>Fresco 0.1.7 Reference Manual</TITLE></HEAD>"
			"<BODY BGCOLOR=#FFC080>"
			"<TABLE WIDTH=100% CELLPADDING=0 CELLSPACING><TR><TD><B><BIG><FONT FACE=helvetica>"
			"&nbsp;%s%s</FONT></BIG></B>\n"
			"</TD><TD ALIGN=RIGHT><FONT FACE=helvetica>[<A HREF=\"index.html\">index</A>]</FONT></TD></TR></TABLE>\n"
			"<HR><P>\n",
			module,buffer);

		fprintf(out,"<B><FONT FACE=helvetica>declared in file:</FONT></A></B> %s<BR>",filename);

		line+=strspn(line, " \t\r\n");
		if (*line==':')
		{
			line++;
			fprintf(out,"<B><FONT FACE=helvetica>ancestors:</FONT></B> ");
			while(1)
			{
				line+=strspn(line, " \t\r\n");
				if (*line == '{')
					break;
				if (*line == ',')
				{
					fprintf(out, ", ");
					line++;
					continue;
				}
				p = buffer;
				if (strncmp(line,"public",5)==0)
				{
					line+=5;
					line+=strspn(line, " \t\r\n");
				}
				while(1)
				{
					*p = *line;
					if ( !isalpha(*line) && !isdigit(*line) && *line!='_' && *line!=':')
						break;
					p++;
					line++;
				}
				*p=0;
				fprintf(out,"<A HREF=\"%s.html\">%s</A>",buffer,buffer);
			}
		}
		fprintf(out, "<P>");
		return;
	}

	if (strncmp(line,"attribute",9)==0)
	{
		line+=9;
		color="#808080";
	}
	else if (strncmp(line,"class",5)==0)
	{
		line+=5;
		color="#0080FF";
	} else
	{
		color="#FF8000";
	}

	p = buffer;
	while(1)
	{
		while(*line!=0)
		{
			if (*line=='}')
			{
				insert--;
				if (insert<0)
				{
					printbar(color,buffer);
					ungetline=-1;
					return;
				}
				strcpy(p,"<BR>");
				p+=4;
			}
			*p = *line;
			p++;
			if(*line==';' || *line=='{')
			{
				if (*line=='{')
					insert++;
				strcpy(p,"<BR>");
				p+=4;
				for(i=0; i<insert; i++)
				{
					strcpy(p, "&nbsp;&nbsp;");
					p+=12;
				}
			}
			
			line++;
		}
		line = read_line();
		if (feof(in) || strncmp(line,"//.",3)==0 || strncmp(line,"//-",3)==0 )
		{
			printbar(color,buffer);
			ungetline=-1;
			return;
		}
	}
}
