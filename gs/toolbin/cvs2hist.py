#!/usr/bin/env python

#    Copyright (C) 2000 Aladdin Enterprises.  All rights reserved.
# 
# This file is part of AFPL Ghostscript.
# 
# AFPL Ghostscript is distributed with NO WARRANTY OF ANY KIND.  No author or
# distributor accepts any responsibility for the consequences of using it, or
# for whether it serves any particular purpose or works at all, unless he or
# she says so in writing.  Refer to the Aladdin Free Public License (the
# "License") for full details.
# 
# Every copy of AFPL Ghostscript must include a copy of the License, normally
# in a plain ASCII text file named PUBLIC.  The License grants you the right
# to copy, modify and redistribute AFPL Ghostscript, but only under certain
# conditions described in the License.  Among other things, the License
# requires that the copyright notice and this notice be preserved on all
# copies.

# $Id$

# Convert the change notices in a CVS repository to the Ghostscript
# History.htm file format.  Based on cvs2log.py by Henry Stiles
# <henrys@artifex.com>.

# ---------------- Generic utilities ---------------- #

# Convert a date/time string in RCS format (yyyy/mm/dd hh:mm:ss) to
# a time in seconds since the epoch.  Note that the result is local time,
# since that is what the only available function (mktime) returns.
def RCSDateToSeconds(date):
    import string, time
    (date_part, time_part) = string.split(date)
    (year, month, day) = string.splitfields(date_part, '/')
    (hour, minute, second) = string.splitfields(time_part, ':')
    tuple = (string.atoi(year), string.atoi(month), string.atoi(day),
             string.atoi(hour), string.atoi(minute), string.atoi(second),
             0, 0, -1)
    return time.mktime(tuple)

# Create line-broken text from a list of items to be blank-separated.
def LineBrokenText(indent, line_length, items):
    leading_space = ' ' * indent
    pos = -1
    lines = leading_space
    # Handle a leading tab or newline in items
    first_char = items[0][0]
    if first_char == '\n':
        pos = pos - 1  # character has width 0
    elif first_char == '\t':
        pos = pos + 7  # character has width 8
    for item in items:
        if pos + 1 + len(item) > line_length:
            lines = lines + '\n' + leading_space + item
            pos = len(item)
        else:
            lines = lines + ' ' + item
            pos = pos + 1 + len(item)
    return lines[1:] + '\n'  # delete the unwanted first space

# 'Normalize' the text of a named anchor to comply with spec
def NormalizeAnchor(name):
    import re
    return re.sub('[^0-9a-zA-Z-_\.]', '_', name)

# replace special characters with html entities
# FIXME: isn't there a library call for this?
def HTMLEncode(line):
    import string
    line = string.join(string.split(line,'&'),'&amp;')
    line = string.join(string.split(line,'<'),'&lt;')
    line = string.join(string.split(line,'>'),'&gt;')
    return line

# ---------------- CVS-specific code ---------------- #

# Return the CVS repository root directory (the argument for -d in CVS
# commands).
def GetCVSRepository():
    try:
	fp = open('CVS/Root', 'r')
    except:
        print "Error: Cannot find CVS/Root"
        return None
    # get the Root name and strip off the newline
    repos = fp.readline()[:-1]
    fp.close()
    return repos
        
# Scan int.mak and lib.mak to find source files associated with the
# interpreter and library respectively.
def ScanMakefileForSources(filename, dict, value):
    import re

    try:
        input = open(filename, 'r')
    except:
        try:
            input = open('gs/' + filename, 'r')
        except:
            print "Error: Unable to open " + filename
            return dict
    lines = input.readlines()
    input.close()
    pattern = re.compile("[a-zA-Z][a-zA-Z0-9_]+[.][ch]")
    for line in lines:
        found = pattern.search(line)
        if found != None:
            dict['src/' + found.group()] = value
    return dict

# Classify a source file name according to what group it should go in.
# This is very specific to Ghostscript.
# Eventually we will replace this with separate subdirectories.
import re
SourceGroupPatterns = map(lambda pair: (re.compile(pair[0]), pair[1]), [
    # Note that Python regex left-anchors the match: an explicit .* is
    # needed for a floating match.
    ["^doc/", "Documentation"],
    ["^examples/", "Interpreter"],
    ["^man/", "Documentation"],
    ["^toolbin/", "Procedures"],
    ["^lib/pdf_.*[.]ps$", "PDF Interpreter"],
    ["^lib/gs_.*[.]ps$", "Interpreter"],
    ["^lib/ht_.*[.]ps$", "Interpreter"],
    ["^lib/.*[.]upp$", "Drivers"],
    ["^lib/.*[.]x[bp]m$", "Interpreter"],
    ["^lib/", "Utilities"],
    ["^src/.*[.]bat$", "Procedures"],
    ["^src/.*[.]cfg$", "Procedures"],
    ["^src/.*[.]com$", "Procedures"],
    ["^src/.*[.]cmd$", "Procedures"],
    ["^src/.*[.]def$", "Procedures"],
    ["^src/.*[.]m[am]k$", "Procedures"],
    ["^src/.*[.]rc$", "Procedures"],
    ["^src/.*[.]rps$", "Procedures"],
    ["^src/.*[.]sh$", "Utilities"],
    ["^src/gdev", "Drivers"],
    ["^src/gen", "Utilities"],
    ["^src/rpm", "Procedures"],
    ["^src/[^.]*$", "Utilities"],
    ["^src/d[pw]", "Interpreter"],
    ["^src/.*[.]cpp$", "Interpreter"],
    ["^src/.*[.]c$", "Utilities"],
    ["", "Other"]    # This pattern must appear at the end.
    ])
def SourceFileGroup(filename, sources):
    if sources.has_key(filename):
        return sources[filename]
    for pattern, group in SourceGroupPatterns:
        if pattern.match(filename) != None:
            return group

# Create a version TOC.
def VersionTOC(version, version_date, groups):
    start = '<ul>\n<li>'
    toc = ''
    for group in groups:
        toc = toc + '    <a href="#' + NormalizeAnchor(version + '-' + group) + '">' + group + '</a>,\n'
    return start + toc[4:-2] + '\n</ul>\n'

# Create a change log group header.
def ChangeLogGroupHeader(group, previous_group, version):
    header = '\n<h2><a name="' + NormalizeAnchor(version + '-' + group) + '"></a>' + group + '</h2><pre>'
    if previous_group != None:
        header = '\n</pre>' + header[1:]
    return header

# Create a change log section header.
# Section 0 = fixes, section 1 = other.
# Return (section header, line prefix)
def ChangeLogSectionHeader(section, previous_section, version):
    if section == 0:
        return ("\nFixes problems:", "\t- ")
    return (None, "\n")

# Build the text for a patch.  (Not really implemented yet.)
def BuildPatch(cvs_command, revision, rcs_file):
    import os, string
    # NB this needs work we only handle the special cases here.
    rev_int_str = revision[:string.find(revision, '.')]
    rev_frac_str = revision[string.find(revision, '.')+1:]
    try:
        prev_frac_int = string.atoi(rev_frac_str) - 1
    except:
        return "the patch must be created manually"
    prev_revision = rev_int_str + '.' + `prev_frac_int`
    patch_command = cvs_command + ' diff -C2 -r' + revision + ' -r' + prev_revision + ' ' + rcs_file
    return os.popen(patch_command, 'r').readlines()

# Create an individual history entry.
def ChangeLogEntry(cvs_command, author, date, rev_files, description_lines, prefix, indent, line_length, patch, text_option):
    import string, time
    # Add the description.
    description = ''
    for line in description_lines:
        description = description + line[:-1] + ' '  # drop trailing \n
    if text_option == 0:
	entry = string.split(string.strip(HTMLEncode(description)))
    else:
	entry = string.split(string.strip(description))
    entry[0] = prefix + entry[0]
    # Add the list of RCS files and revisions.
    items = []
    for revision, rcs_file in rev_files:
        if rcs_file[:4] == 'src/':
            rcs_file = rcs_file[4:]
        items.append(rcs_file + ' [' + revision + ']' + ',')
    items.sort()
    items[0] = '(' + items[0]
    items[-1] = items[-1][:-1] + ':'
    # Add the date and author.
    entry = entry + items + string.split(date) + [author + ')']
    entry = LineBrokenText(0, line_length, entry)
    # Add on the patches if necessary.
    if ( patch == 1 ):
        for revision, rcs_file in rev_files:
            for patch_line in BuildPatch(cvs_command, revision, rcs_file):
                entry = entry + patch_line
    return entry

# Build the combined CVS log.  We return an array of tuples of
# (date, author, description, rcs_file, revision, tags).
# The date is just a string in RCS format (yyyy/mm/dd hh:mm:ss).
# The description is a sequence of text lines, each terminated with \n.
def BuildLog(log_date_command):
    import os, re, string

    reading_description = 0
    reading_tags = 0
    description = []
    log = []
    tag_pattern = re.compile("^	([^:]+): ([0-9.]+)\n$")

    for line in os.popen(log_date_command, 'r').readlines():
	if line[:5] == '=====' or line[:5] == '-----':
	    if description != []:
                try:
                    my_tags = tags[revision]
                except KeyError:
                    my_tags = []
		log.append((date, author, description, rcs_file, revision, my_tags))
	    reading_description = 0
	    description = []
            continue
	if reading_description:
            # Omit initial empty description lines.
            if line == '\n' and description == []:
                continue
	    description.append(line)
            continue
        if reading_tags:
            match = tag_pattern.match(line)
            if match == None:
                reading_tags = 0
                continue
            tag = match.group(1)
            revs = string.splitfields(match.group(2), ", ")
            for rev in revs:
                try:
                    tags[rev].append(tag)
                except KeyError:
                    tags[rev] = [tag]
            continue
	if line[:len("Working file: ")] == "Working file: ":
	    rcs_file = line[len("Working file: "):-1]
            tags = {}
	elif line[:len("revision ")] == "revision ":
	    revision = line[len("revision "):-1]
	elif line[:len("date: ")] == "date: ":
	    (dd, aa, ss, ll) = string.splitfields(line, ';')
	    (discard, date) = string.splitfields(dd, ': ')
	    (discard, author) = string.splitfields(aa, ': ')
	    reading_description = 1
        elif line[:len("symbolic names:")] == "symbolic names:":
            reading_tags = 1

    for entry in log:
        print entry
    print '----------------------------------------------------------------'

    return log

# ---------------- Main program ---------------- #

# make sure the group names normalize to distinct anchors!
GroupOrder = {
    "Documentation" : 1,
    "Procedures" : 2,
    "Utilities" : 3,
    "Drivers" : 4,
    "Platforms" : 5,
    "Fonts" : 6,
    "PDF writer" : 7,
    "PDF Interpreter" : 8,
    "Interpreter" : 9,
    "Streams" : 10,
    "Library" : 11,
    "Other" : 12
    }

# Parse command line options and build logs.
def main():
    import sys, getopt, time, string, re
    try:
	opts, args = getopt.getopt(sys.argv[1:], "C:d:Hi:l:Mptr:v:",
				   ["CVS_command",
				    "date",
                                    "indent",
                                    "length",
                                    "Merge",
				    "patches",     #### not yet supported
				    "text",
				    "rlog_options", #### not yet supported
                                    "version"
				    ])

    except getopt.error, msg:
	sys.stdout = sys.stderr
	print msg
	print "Usage: cvs2hist ...options..."
	print "Options: [-C CVS_command] [-d rcs_date] [-i indent] [-l length]"
	print "         [-M] [-p] [-t] [-r rlog_options] [-v version]"
	sys.exit(2)

    # Set up defaults for all of the command line options.
    cvs_repository = GetCVSRepository()
    if not cvs_repository:
	print "cvs2hist must be executed in a working CVS directory"
	sys.exit(2)
    cvs_command = "cvs"
    date_option = ""
    indent = 0
    length = 76
    merge = 0
    patches = 0
    rlog_options = ""
    text_option = 0;
    version = "CVS"
    # override defaults if specified on the command line
    for o, a in opts:
	if o == '-C' : cvs_command = a
	elif o == '-d' : date_option = "'-d>" + a + "'"
	elif o == '-i' : indent = string.atoi(a)
	elif o == '-l' : length = string.atoi(a)
        elif o == '-M' : merge = 1
        elif o == '-p' : patches = 1
	elif o == '-t' : text_option = 1
	elif o == '-r' : rlog_options = a
	elif o == '-v' : version = a
	else: print "getopt should have failed already"

    # return only messages on the default branch, unless told otherwise
    if rlog_options == "":
	rlog_options = '-b'
    # set up the cvs log command arguments.
    log_date_command = cvs_command + ' -d ' + cvs_repository +' -Q log ' + date_option + ' ' + rlog_options
    # Acquire the log data.
    log = BuildLog(log_date_command)
    # By default, if no date option is specified, produce output for
    # changes since the most recent tagged version.  To avoid a second
    # pass over the CVS repository, we do the filtering locally.
    min_date = '0000/00/00 00:00:00'
    if date_option == "":
        for date, author, text_lines, file, revision, tags in log:
            if len(tags) != 0 and date > min_date:
                min_date = date
    # Scan the makefiles to find source file names.
    sources = {}
    sources = ScanMakefileForSources('src/lib.mak', sources, "Library")
    for key in sources.keys():
        if key[:5] == 'src/s':
            sources[key] = "Streams"
    sources = ScanMakefileForSources('src/devs.mak', sources, "Drivers")
    for key in sources.keys():
        if key[:10] == 'src/gdevpd':
            sources[key] = "PDF writer"
    sources = ScanMakefileForSources('src/int.mak', sources, "Interpreter")
    sources = ScanMakefileForSources('src/contrib.mak', sources, "Drivers")
    # Sort the log by group, then by fix/non-fix, then by date, then by
    # description (to group logically connected files together).
    sorter = []
    group_pattern = re.compile("^(\([^)]+\))[ ]+")
    for date, author, text_lines, file, revision, tags in log:
        if date <= min_date:
            continue
        line = ''
        while len(text_lines) > 0:
            line = string.strip(text_lines[0])
            if line != '':
                break
            text_lines[:1] = []
        if merge:
            group = "(all)"
        elif group_pattern.match(text_lines[0]) != None:
            match = group_pattern.match(text_lines[0])
            group = match.group(1)
            text_lines[0] = text_lines[0][len(match.group(0)):]
        else:
            group = SourceFileGroup(file, sources)
        try:
            group_order = GroupOrder[group]
        except KeyError:
            group_order = 99
        if line[:4] == "Fix:":
            text_lines[0] = line[4:] + '\n'
            section = 0
        else:
            section = re.match("^Fix", text_lines[0]) < 0
        sorter.append((group_order, section, date, group, text_lines, author, file, revision, tags))
    sorter.sort()
    log = sorter
    # Print the HTML header.
    time_now = time.localtime(time.time())
    version_date = time.strftime('%Y-%m-%d', time_now)
    version_time = time.strftime('%Y-%m-%d %H:%M:%S', time_now)
    if text_option == 0:
	print "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">"
        print "<html><head>"
        print "<title>Ghostscript " + version + " change history as of " + version_time + "</title>"
	print "<link rel=stylesheet type=\"text/css\" href=\"gs.css\">"
        print "</head><body>\n"
	print '<!-- cvs command: ' + log_date_command + ' -->\n'

        last_group = None
        groups = []
        for omit_group_order, section, date, group, description, author, rcs_file, revision, tags in log:
            if group != last_group:
                groups.append(group)
                last_group = group
        print VersionTOC(version, version_date, groups)
    else:
        print "Ghostscript change history as of " + version_time
    # Pass through the logs creating new entries based on changing
    # authors, groups, dates and descriptions.
    last_group = None
    last_section = None
    last_description = None
    last_author = None
    last_date = None
    rev_files = []
    for omit_group_order, section, date, group, description, author, rcs_file, revision, tags in log:
        if group != last_group:
            if rev_files != []:
                print ChangeLogEntry(cvs_command, last_author, last_date, rev_files, last_description, prefix, indent, length, patches, text_option)[:-1]
                rev_files = []
	    if text_option == 0:
		print ChangeLogGroupHeader(group, last_group, version)
	    else:
		print '\n****** ' + group + ' ******'
            last_group = group
            last_section = None
            last_description = None
        if section != last_section:
            if rev_files != []:
                print ChangeLogEntry(cvs_command, last_author, last_date, rev_files, last_description, prefix, indent, length, patches, text_option)[:-1]
                rev_files = []
	    (header, prefix) = ChangeLogSectionHeader(section, last_section, version)
            if header != None:
                print header
            last_section = section
            last_description = None
	if author != last_author or description != last_description or abs(RCSDateToSeconds(date) - RCSDateToSeconds(last_date)) >= 3:
            if rev_files != []:
                print ChangeLogEntry(cvs_command, last_author, last_date, rev_files, last_description, prefix, indent, length, patches, text_option)[:-1]
                rev_files = []
            last_author = author
            last_date = date
            last_description = description
	# Accumulate the revisions and RCS files.
        rev_files.append((revision, rcs_file))

    # print the last entry if there is one (i.e. the last two entries
    # have the same author and date)
    if rev_files != []:
        print ChangeLogEntry(cvs_command, last_author, last_date, rev_files, last_description, prefix, indent, length, patches, text_option)[:-1]
    # Print the HTML trailer.
    if text_option == 0:
        print "\n</pre></body></html>"

if __name__ == '__main__':
    main()
