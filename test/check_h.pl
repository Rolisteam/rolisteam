#!/bin/perl
my $file;
my $line;
my $tab;
my $error;
my @l;
my @final;

foreach $arg(@ARGV)
{
	if ($arg !~ m/.h$/)
	{
		print "$arg: not a header file.";
		next ;
	}

	$line = 0;
	$tab = 0;
	$file = $arg;
	$error = 0;
	@final = ();
	checkFile($arg);
	if (copyFile($arg))
	{
		correctFile($arg);
	}
}

sub correctFile
{
	my ($path) = @_;
	
	if (!open(FILE, ">$path"))
	{
		print "$path: $!\n";
		return 0;
	}
	foreach $tmp (@final)
	{
		print FILE "$tmp\n";
	}
}

sub copyFile
{
	my ($path) = @_;
	
	if (!open(BACKUP, ">$path.backup"))
	{
		print "$path: $!\n";
		return 0;
	}
	if (!open(FILE, "<$path"))
	{
		close(BACKUP);
		print "$path: $!\n";
		return 0;
	}
	while (defined($l = <FILE>))
	{
		print BACKUP "$l";
	}
	close(BACKUP);
	close(FILE);
	return 1;
}

sub checkFile
{
	my ($path) = @_;
	
	if (!open(FILE, "<$path"))
	{
		print "$path: $!\n";
	}
	else
	{
		@l = <FILE>;
		isFile();
		if ($error == 0)
		{
			print "[$file] : No error.\n";
		}
		else
		{
			print "[$file] : $error errors.\n";
		}
	}
	close(FILE);
}

sub affError
{
	my ($errorMessage) = @_;
	my $currentLine = $line + 1;
	$error++;
	
	print "[$file] l.$currentLine : $errorMessage\n";
}

sub trim
{
	my ($ref) = @_;
	
	chomp $$ref;
	if ($$ref =~ s/^(\t+)//)
	{
		affError("Tabulation for indentation.");
	}
	elsif ($$ref =~ s/^( +)//)
	{
		if ($$ref =~ m/^};?$/)
		{
			$tab--;
		}
		if ((length $1) > ($tab * 4) && !$hideTab)
		{
			affError("Too many indentation's spaces.");
		}
		elsif ((length $1) < ($tab * 4) && !$hideTab)
		{
			affError("Not enaugh indentation's spaces.");
		}
	}
	elsif (length $$ref > 0)
	{
		if ($$ref =~ m/^};?$/)
		{
			$tab--;
		}
		if (($tab * 4) > 0 && $$ref !~ m/:$/)
		{
			affError("Not enaugh indentation's spaces.");
		}
	}
	if ($$ref =~ m/^{$/)
	{
		$tab++;
	}
}

sub nextLine
{
	do
	{
		$line++;
		trim(\$l[$line]);
		correctLine();
	}
	while (defined($l[$line]) && length $l[$line] == 0);
	if (!defined($l[$line]))
	{
		return 0;
	}
	return 1;
}

sub correctLine
{
	$tmp = $l[$line];
	if ($tmp !~ m/\/\*\*/ && $tmp !~ m/\*\// && $tmp !~ m/^\s*\*/)
	{
		if ($tmp =~ m/^for/)
		{
			$tmp =~ s/\s*;\s*/; /g;
		}
		$tmp =~ s/\s*\*\s*/\* /g;
		$tmp =~ s/\s*&\s*/& /g;
		$tmp =~ s/\s*:/:/g;
		$tmp =~ s/\s*\(\s*/\(/g;
		$tmp =~ s/\s*\)/\)/g;
		$tmp =~ s/\[\s*/\[/g;
		$tmp =~ s/\s*\]/\]/g;
		$tmp =~ s/\<\s*/\</g;
		$tmp =~ s/\s*\>/\>/g;
		$tmp =~ s/\s*,\s*/, /g;
	}
	if ($tmp =~ m/{$/)
	{
		$tmp =  " " x (($tab - 1) * 4) . $tmp;
	}
	else
	{
		$tmp =  " " x ($tab * 4) . $tmp;
	}
	$final[$line] = $tmp;
}

sub isFile
{
	$line--;
	nextLine();
	isLicense();
	isIfndef();
	isIncludes();
	while (isComment() || isClassDeclaration() || isClass() || isNamespace() || isInclude())
	{
		if (isInclude())
		{
			affError("Wrong placed include.");
		}
		nextLine();
	}
	isEndif();
}

sub isMultiLineComment
{
	if ($l[$line] =~ m-^/\*-)
	{
		while ($l[$line] !~ m-\*/-)
		{
			nextLine();
		}
		if ($l[$line] !~ m-\*/$-)
		{
			affError("Code after commentary.");
		}
		return 1;
	}
	return 0;
}

sub isSoloLineComment
{
	if ($l[$line] =~ m-^//-)
	{
		return 1;
	}
	return 0;
}

sub isComment
{
	if (isMultiLineComment() || isSoloLineComment())
	{
		return 1;
	}
	return 0;
}

sub isLicense
{
	if (isMultiLineComment())
	{
		nextLine();
		return 1;
	}
	affError("Missing license.");
	return 0;
}

sub isIfndef
{
	while (isComment())
	{
		nextLine();
	}
	if ($l[$line] !~ m/^#ifndef/)
	{
		affError("Missing #ifndef for multiple inclusions protection.");
		return 0;
	}
	nextLine();
	if ($l[$line] !~ m/^#define/)
	{
		affError("Missing #define for multiple inclusions protection.");
		return 0;
	}
	nextLine();
	return 1;
}

sub isSystemInclude
{
	if ($l[$line] =~ m/^#include <[^>]+>/)
	{
		return 1;
	}
	return 0;
}

sub isPersonalInclude
{
	if ($l[$line] =~ m/^#include "[^"]+"/)
	{
		return 1;
	}
	return 0;
}

sub isInclude
{
	if (isSystemInclude() || isPersonalInclude())
	{
		return 1;
	}
	return 0;
}

sub isIncludes
{
	$includeError = 0;
	while (isSystemInclude() || isComment())
	{
		nextLine();
	}
	while (isPersonalInclude() || isComment())
	{
		nextLine();
	}
	while (isInclude() || isComment())
	{
		if (!$includeError)
		{
			affError("System include after personal include.");
			$includeError = 1;
		}
		nextLine();
	}
}

sub isNamespace
{
	if ($l[$line] =~ m/^namespace \w+\s*({?)/)
	{
		if ($1)
		{
			$tab++;
		}
		return 1;
	}
	if ($l[$line] =~ m/^{/)
	{
		return 1;
	}
	if ($l[$line] =~ m/^}/)
	{
		return 1;
	}
	return 0;
}

sub isClassDeclaration
{
	if ($l[$line] =~ m/^class \w+;/)
	{
		return 1;
	}
	return 0;
}

sub isClassName
{
	if ($l[$line] !~ m/^class \w+(?: : (?:public|private) \w+(?:, ?(?:public|private) \w+)*)?\s*({?)/)
	{
		affError("No class declaration.");
		return 0;
	}
	nextLine();
	if ($1)
	{
		$tab++;
		affError("\"{\" must be on a new line.");
	}
	elsif ($l[$line] !~ m/^{/)
	{
		affError("\"{\" expecting.");
	}
	nextLine();
	return 1;
}

sub isQobject
{
	if ($l[$line] =~ m/^Q_OBJECT/)
	{
		nextLine();
		return 1;
	}
	return 0;
}

sub isLabel
{
	my $tmp = 0;
	if ($l[$line] =~ m/^(public)( )?:/)
	{
		$tmp = 1;
	}
	elsif ($l[$line] =~ m/^(public slots)( )?:/)
	{
		$tmp = 2;
	}
	elsif ($l[$line] =~ m/^(signals)( )?:/)
	{
		$tmp = 3;
	}
	elsif ($l[$line] =~ m/^(protected)( )?:/)
	{
		$tmp = 4;
	}
	elsif ($l[$line] =~ m/^(protected slots)( )?:/)
	{
		$tmp = 5;
	}
	elsif ($l[$line] =~ m/^(private)( )?:/)
	{
		$tmp = 6;
	}
	elsif ($l[$line] =~ m/^(private slots)( )?:/)
	{
		$tmp = 7;
	}
	if ($2 eq " ")
	{
		affError("No space between \"$1\" and \":\".");
	}
	return $tmp;
}

sub testArg
{
	my ($args) = @_;
	
	my $nb = 0;
	my @arg = split(m/,/, $args);
	foreach $var(@arg)
	{
		if ($nb != 0 && $var !~ m/^ /)
		{
			affError("Missing space after \",\".");
		}
		$nb++;
		if ($var =~ m/^(\w+\s+)?[\w:<>]+(\s*)([\*\&]*)(\s*)(\w+)/)
		{
			my $s2 = $2;
			my $s3 = $3;
			my $s4 = $4;
			
			if ($s2 && $s3)
			{
				affError("Space before \"$3\".");
			}
			if ($s3 && !$s4)
			{
				affError("No space after \"$3\".");
			}
			return 1;
		}
	}
}

sub testBrackets
{
	my ($s1, $s2, $s3) = @_;
	
	if ($s1 =~ m/\s/)
	{
		affError("No space before \"(\".");
	}
	if ($s2 =~ m/\s/)
	{
		affError("No space after \"(\".");
	}
	if ($s3 =~ m/\s$/)
	{
		affError("No space before \")\".");
	}
}

sub isConstructor
{
	if ($l[$line] =~ m/^\w+(\s*)\((\s*)?([^\)]*)\)/)
	{
		testBrackets($1, $2, $3);
		testArg($3);
		return 1;
	}
	return 0;
}

sub isDestructor
{
	if ($l[$line] =~ m/^~\w+(\s*)\((\s*)([^\)]*)\)/)
	{
		testBrackets($1, $2, $3);
		testArg($3);
		return 1;
	}
	return 0;
}

sub isFunction
{
	if ($l[$line] =~ m/^((?:static|virtual)\s+)?[\w:]+(?:<[^>]*>)?(\s*)([\*\&]*)(\s*)\w+(\s*)\((\s*)([^\)]*)\)/)
	{
		if ($2 && $3)
		{
			affError("Space before \"$3\".");
		}
		if ($3 && !$4)
		{
			affError("No space after \"$3\".");
		}
		my $s7 = $7;
		testBrackets($5, $6, $7);
		testArg($s7);
		return 1;
	}
	return 0;
}

sub isVariable
{
	if ($l[$line] =~ m/^(static\s+)?[\w:]+(?:<[^>]*>)?(\s*)([\*\&]*)(\s*)([\w\[\]]+)\s*;/)
	{
		if ($2 && $3)
		{
			affError("Space before \"$3\".");
		}
		if ($3 && !$4)
		{
			affError("No space after \"$3\".");
		}
		
		if ($5 !~ m/^m_/)
		{
			affError("Variable members must start with \"m_\".");
		}
		return 1;
	}
	return 0;
}

sub isEnum
{
	if ($l[$line] =~ m/^enum \w+(\s*){(\s*)([^}]*)}/)
	{
		if (!$1)
		{
			affError("No space before \"{\"");
		}
		if ($2)
		{
			affError("Space after \"{\"");
		}
		my $args = $3;
		if ($3 =~ m/\s$/)
		{
			affError("Space before \"}\"");
		}
		my @argTab = split(m/,/, $args);
		my $nb = 0;
		foreach $arg(@argTab)
		{
			if ($nb != 0)
			{
				if ($arg !~ m/\s/)
				{
					affError("No space after \",\".");
				}
			}
			$nb++;
		}
		return (1);
	}
	return 0;
}

sub isClassEnd
{
	if ($l[$line] !~ m/^}/)
	{
		return 0;
	}
	return 1;
}

sub isClass
{
	$label = 1;
	if ($l[$line] !~ m/^class \w+(?: : (?:public|private) \w+(?:, (?:public|private) \w+)*)?(;?)/ || $2)
	{
		return 0;
	}
	isClassName();
	isQobject();
	while (1)
	{
		my $tmpLabel = isLabel();
		if ($tmpLabel != 0)
		{
			if ($tmpLabel < $label && $label < 7)
			{
				affError("Wrong order.");
			}
			$label = $tmpLabel;
			nextLine();
		}
		elsif (isConstructor() || isDestructor()
				|| isFunction() || isVariable()
				|| isEnum() || isComment())
		{
			nextLine();
		}
		elsif (isClassEnd())
		{
			last ;
		}
		else
		{
			affError("Unknowed line.");
			nextLine();
		}
	}
	return 1;
}

sub isEndif()
{
	if ($l[$line] !~ m/^#endif/)
	{
		affError("Missing #endif.");
	}
}
