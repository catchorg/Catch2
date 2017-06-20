import re

preprocessorRe = re.compile( r'\s*#.*' )

fdefineRe = re.compile( r'\s*#\s*define\s*(\S*)\s*\(' ) # #defines that take arguments
defineRe = re.compile( r'\s*#\s*define\s*(\S*)(\s+)(.*)' ) # all #defines
undefRe = re.compile( r'\s*#\s*undef\s*(\S*)' ) # all #undefs

ifdefCommonRe = re.compile( r'\s*#\s*if' ) # all #ifdefs
ifdefRe = re.compile( r'\s*#\s*ifdef\s*(\S*)' )
ifndefRe = re.compile( r'\s*#\s*ifndef\s*(\S*)' )
endifRe = re.compile( r'\s*#\s*endif\s*//\s*(.*)' )
elseRe = re.compile( r'\s*#\s*else' )
ifRe = re.compile( r'\s*#\s*if\s+(.*)' )

nsRe = re.compile( r'(.*?\s*\s*namespace\s+)(\w+)(\s*{?)(.*)' )
nsCloseRe = re.compile( r'(.*\s*})(\s*\/\/\s*namespace\s+)(\w+)(\s*)(.*)' )


class LineMapper:
    def __init__( self, idMap, outerNamespace ):
        self.idMap = idMap
        self.outerNamespace = outerNamespace

    def replaceId( self, lineNo, id ):
        if not self.idMap.has_key( id ):
            raise ValueError( "Unrecognised macro identifier: '{0}' on line: {1}".format( id, lineNo ) )
        subst = self.idMap[id]
        if subst == "":
            return id
        else:
            return subst

    # TBD:
    #  #if, #ifdef, comments after #else
    def mapLine( self, lineNo, line ):
        m = ifndefRe.match( line )
        if m:
            return "#ifndef " + self.replaceId( lineNo, m.group(1)) + "\n"
        m = defineRe.match( line )
        if m:
            return "#define {0}{1}{2}\n".format( self.replaceId( lineNo, m.group(1)), m.group(2), m.group(3) )
        m = endifRe.match( line )
        if m:
            return "#endif // " + self.replaceId( lineNo, m.group(1)) + "\n"
        m = nsCloseRe.match( line )
        if m:
            originalNs = m.group(3)
            # print("[{0}] originalNs: '{1}' - closing".format(lineNo, originalNs))
            # print( "  " + line )
            # print( "  1:[{0}]\n  2:[{1}]\n  3:[{2}]\n  4:[{3}]\n  5:[{4}]".format( m.group(1), m.group(2), m.group(3), m.group(4), m.group(5) ) )
            if self.outerNamespace.has_key(originalNs):
                outerNs, innerNs = self.outerNamespace[originalNs]
                return "{0}}}{1}{2}::{3}{4}{5}\n".format( m.group(1), m.group(2), outerNs, innerNs, m.group(4), m.group(5))
        m = nsRe.match( line )
        if m:
            originalNs = m.group(2)
            # print("[{0}] originalNs: '{1}'".format(lineNo, originalNs))
            # print( "  " + line )
            # print( "  1:[{0}]\n  2:[{1}]\n  3:[{2}]\n  4:[{3}]".format( m.group(1), m.group(2), m.group(3), m.group(4) ) )
            if self.outerNamespace.has_key(originalNs):
                outerNs, innerNs = self.outerNamespace[originalNs]
                return "{0}{1} {{ namespace {2}{3}{4}\n".format( m.group(1), outerNs, innerNs, m.group(3), m.group(4) )
        return line

    def mapFile(self, filenameIn, filenameOut ):
        print( "Embedding:\n  {0}\nas:\n  {1}".format( filenameIn, filenameOut ) )
        with open( filenameIn, 'r' ) as f, open( filenameOut, 'w' ) as outf:
            lineNo = 1
            for line in f:
                outf.write( self.mapLine( lineNo, line ) )
                lineNo = lineNo + 1
        print( "Written {0} lines".format( lineNo ) )