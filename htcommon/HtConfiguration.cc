//
// HtConfiguration.cc
//
//             HtConfiguration: extends Configuration class
//             to implement Apache-style config. Uses parser
//             generated by Bison from conf_parser.yxx
//
// Part of the ht://Dig package   <http://www.htdig.org/>
// Copyright (c) 1999 The ht://Dig Group
// For copyright details, see the file COPYING in your distribution
// or the GNU Public License version 2 or later
// <http://www.gnu.org/copyleft/gpl.html>
//
// $Id: HtConfiguration.cc,v 1.1.2.8 2000/02/16 14:32:41 vadim Exp $
//

#include <stdio.h>
#include"HtConfiguration.h"
#include "ParsedString.h"
#include <stdlib.h>
#include <fstream.h>
#include <stdlib.h>
#include <ctype.h>
#include <locale.h>


//********************************************************************
//  Add complex entry to the configuration
//
void
HtConfiguration::Add(char *name, char *value, Configuration *aList) {

  if (strcmp("url",name)==0) {  //add URL entry
    URL tmpUrl(strdup(value));
    if (Dictionary *paths=(Dictionary *)dcUrls[tmpUrl.host()]) {
      paths->Add(tmpUrl.path(),aList);
    } else {
      paths=new Dictionary();
      paths->Add(tmpUrl.path(),aList);
      dcUrls.Add(tmpUrl.host(),paths);
    }
    } else {

      Object *treeEntry=dcBlocks[name];
      if (treeEntry!=NULL) {
	((Dictionary *)treeEntry)->Add(value,aList);
      } else {
	treeEntry=new Dictionary(16);
	((Dictionary *)treeEntry)->Add(value,aList);
	dcBlocks.Add(name, treeEntry);
      }
    }
}

//*********************************************************************
const String HtConfiguration::Find(const char *blockName,const char *name,const char *value) const
{
  if (!(blockName && name && value) )
    return String();
  union {
    void      *ptr;
    Object *obj;
    Dictionary *dict;
    HtConfiguration *conf;
  } tmpPtr;
  String chr;
  
  if (strcmp("url",blockName)==0) { // URL needs special compare
    URL paramUrl(name);     // split URL to compare separatly host and path
    chr=Find(&paramUrl,value);
    if (chr[0]!=0) {
      return chr;
    }
    }
    else { // end "server"
      tmpPtr.obj=dcBlocks.Find(blockName);
      if (tmpPtr.ptr) {
	tmpPtr.obj = tmpPtr.dict->Find(name);
	if (tmpPtr.ptr) {
	  chr = tmpPtr.conf->Find(value);
	  if (chr[0] != 0)
		return chr;
	}
      }
    }
 
  // If this parameter is defined in global then return it
  chr=Find(value);
  if (chr[0]!=0) {
    return chr;
  }
#ifdef DEBUG
  cerr << "Could not find configuration option " << blockName<<":"
       <<name<<":"<<value<< "\n";
#endif
  return String();
}

//*********************************************************************
//
const String HtConfiguration::Find(URL *aUrl, const char *value) const
{
 if (!aUrl)
    return String();
  Dictionary *tmpPtr=(Dictionary *)dcUrls.Find( aUrl->host() );
  if (tmpPtr) {       // We've got such host in config
    tmpPtr->Start_Get();
    // Try to find best matched URL
    //
    struct {
      Object		*obj;
      unsigned int	len;
      String		value;
    } candidate;
    candidate.len=0; 
    String returnValue;
    // Begin competition: which URL is better?
    //
    // TODO: move this loop into Dictionary
    // (or create Dictionary::FindBest ?)
    // or make url list sorted ?
    // or implement abstract Dictionary::Compare?
    char *strParamUrl=aUrl->path();
    while ( char *confUrl=tmpPtr->Get_Next() ) {   
      if (strncmp(confUrl,strParamUrl,strlen(confUrl))==0 
	  && (strlen(confUrl)>=candidate.len))  {
	// it seems this URL match better
	candidate.obj=tmpPtr->Find(confUrl);
	// but does it has got necessery parameter?
	candidate.value=((HtConfiguration *)candidate.obj)->Find(value);
	if (candidate.value[0]!=0) {
	  // yes, it has! We've got new candidate.
	  returnValue=candidate.value;
	  candidate.len=candidate.value.length();
	}
      }
    }
    if (candidate.len>0) {
      return ParsedString(returnValue).get(dcGlobalVars);
    }
       
  }
  return Find(value);
}


//*********************************************************************
int HtConfiguration::Value(char *blockName,char *name,char *value,
			 int default_value = 0) {
int retValue=default_value;
String tmpStr=Find(blockName,name,value);
 if (tmpStr[0]!=0) {
   retValue=atoi(tmpStr.get());
 }
return retValue;
}

//*********************************************************************
double HtConfiguration::Double(char *blockName,char *name,char *value,
				double default_value = 0) {
double retValue=default_value;
String tmpStr=Find(blockName,name,value);
 if (tmpStr[0]!=0) {
   retValue=atof(tmpStr.get());
 }
return retValue;
}

//*********************************************************************
int HtConfiguration::Boolean(char *blockName,char *name,char *value,
				 int default_value = 0) {
int retValue=default_value;
String tmpStr=Find(blockName,name,value);
 if (tmpStr[0]!=0) {
        if (mystrcasecmp((char*)tmpStr, "true") == 0 ||
            mystrcasecmp((char*)tmpStr, "yes") == 0 ||
            mystrcasecmp((char*)tmpStr, "1") == 0)
            retValue = 1;
        else if (mystrcasecmp((char*)tmpStr, "false") == 0 ||
                 mystrcasecmp((char*)tmpStr, "no") == 0 ||
                 mystrcasecmp((char*)tmpStr, "0") == 0)
            retValue = 0;

 }
return retValue;
}

//*********************************************************************
//*********************************************************************
int HtConfiguration::Value(URL *aUrl,char *value,
			 int default_value = 0) {
int retValue=default_value;
String tmpStr=Find(aUrl,value);
 if (tmpStr[0]!=0) {
   retValue=atoi(tmpStr.get());
 }
return retValue;
}

//*********************************************************************
double HtConfiguration::Double(URL *aUrl,char *value,
				double default_value = 0) {
double retValue=default_value;
String tmpStr=Find(aUrl,value);
 if (tmpStr[0]!=0) {
   retValue=atof(tmpStr.get());
 }
return retValue;
}

//*********************************************************************
int HtConfiguration::Boolean(URL *aUrl,char *value,
				 int default_value = 0) {
int retValue=default_value;
String tmpStr=Find(aUrl,value);
 if (tmpStr[0]!=0) {
        if (mystrcasecmp((char*)tmpStr, "true") == 0 ||
            mystrcasecmp((char*)tmpStr, "yes") == 0 ||
            mystrcasecmp((char*)tmpStr, "1") == 0)
            retValue = 1;
        else if (mystrcasecmp((char*)tmpStr, "false") == 0 ||
                 mystrcasecmp((char*)tmpStr, "no") == 0 ||
                 mystrcasecmp((char*)tmpStr, "0") == 0)
            retValue = 0;

 }
return retValue;
}

//*********************************************************************
//
int 
HtConfiguration::Read(const String& filename)
{
extern FILE* yyin;
extern int yyparse(void*);
if ((yyin=fopen(filename,"r"))==NULL) 
	return NOTOK;

FileName=filename; // need to be before yyparse() because is used in it
yyparse(this);
fclose(yyin);
return OK;
}

//********************************************************************
//
inline
String HtConfiguration::ParseString(const char *str) const {
 return ParsedString(str).get(dcGlobalVars);
}