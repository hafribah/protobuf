#!/usr/bin/env python

import datetime
import re
import sys
from xml.dom import minidom

if len(sys.argv) < 2:
  print """
[ERROR] Please specify a version.

Example:
./update_version.py 2.1.3
"""
  exit(1)

NEW_VERSION = sys.argv[1]
NEW_VERSION_INFO = NEW_VERSION.split('.')
if len(NEW_VERSION_INFO) != 3:
  print """
[ERROR] Version must be in the format <MAJOR>.<MINOR>.<MICRO>

Example:
./update_version.py 2.1.3
"""
  exit(1)


def Find(elem, tagname):
  for child in elem.childNodes:
    if child.nodeName == tagname:
      return child
  return None


def FindAndClone(elem, tagname):
  return Find(elem, tagname).cloneNode(True)


def ReplaceText(elem, text):
  elem.firstChild.replaceWholeText(text)


def RewriteXml(filename, rewriter, add_xml_prefix=True):
  document = minidom.parse(filename)
  rewriter(document)
  # document.toxml() always prepend the XML version without inserting new line.
  # We wants to preserve as much of the original formatting as possible, so we
  # will remove the default XML version and replace it with our custom one when
  # whever necessary.
  content = document.toxml().replace('<?xml version="1.0" ?>', '')
  file_handle = open(filename, 'wb')
  if add_xml_prefix:
    file_handle.write('<?xml version="1.0" encoding="UTF-8"?>\n')
  file_handle.write(content)
  file_handle.close()


def RewriteTextFile(filename, line_rewriter):
  lines = open(filename, 'r').readlines()
  updated_lines = []
  for line in lines:
    updated_lines.append(line_rewriter(line))
  if lines == updated_lines:
    print '%s was not updated. Please double check.' % filename
  f = open(filename, 'w')
  f.write(''.join(updated_lines))
  f.close()


def UpdateConfigure():
  RewriteTextFile('configure.ac',
    lambda line : re.sub(
      r'^AC_INIT\(\[Protocol Buffers\],\[.*\],\[protobuf@googlegroups.com\],\[protobuf\]\)$',
      ('AC_INIT([Protocol Buffers],[%s],[protobuf@googlegroups.com],[protobuf])'
        % NEW_VERSION),
      line))


def UpdateCpp():
  cpp_version = '%s00%s00%s' % (
    NEW_VERSION_INFO[0], NEW_VERSION_INFO[1], NEW_VERSION_INFO[2])
  def RewriteCpp(line):
    line = re.sub(
      r'^#define GOOGLE_PROTOBUF_VERSION .*$',
      '#define GOOGLE_PROTOBUF_VERSION %s' % cpp_version,
      line)
    if NEW_VERSION_INFO[2] == '0':
      line = re.sub(
        r'^#define GOOGLE_PROTOBUF_MIN_LIBRARY_VERSION .*$',
        '#define GOOGLE_PROTOBUF_MIN_LIBRARY_VERSION %s' % cpp_version,
        line)
      line = re.sub(
        r'^#define GOOGLE_PROTOBUF_MIN_PROTOC_VERSION .*$',
        '#define GOOGLE_PROTOBUF_MIN_PROTOC_VERSION %s' % cpp_version,
        line)
      line = re.sub(
        r'^static const int kMinHeaderVersionForLibrary = .*$',
        'static const int kMinHeaderVersionForLibrary = %s;' % cpp_version,
        line)
      line = re.sub(
        r'^static const int kMinHeaderVersionForProtoc = .*$',
        'static const int kMinHeaderVersionForProtoc = %s;' % cpp_version,
        line)
    return line
  RewriteTextFile('src/google/protobuf/stubs/common.h', RewriteCpp)


def UpdateCsharp():
  RewriteXml('csharp/src/Google.Protobuf/Google.Protobuf.csproj',
    lambda document : ReplaceText(
      Find(Find(document.documentElement, 'PropertyGroup'), 'VersionPrefix'),
      NEW_VERSION),
    add_xml_prefix=False)

  RewriteXml('csharp/Google.Protobuf.Tools.nuspec',
    lambda document : ReplaceText(
      Find(Find(document.documentElement, 'metadata'), 'version'),
      NEW_VERSION))


def UpdateJava():
  RewriteXml('java/pom.xml',
    lambda document : ReplaceText(
      Find(document.documentElement, 'version'), NEW_VERSION))

  RewriteXml('java/core/pom.xml',
    lambda document : ReplaceText(
      Find(Find(document.documentElement, 'parent'), 'version'),
      NEW_VERSION))

  RewriteXml('java/util/pom.xml',
    lambda document : ReplaceText(
      Find(Find(document.documentElement, 'parent'), 'version'),
      NEW_VERSION))

  RewriteXml('protoc-artifacts/pom.xml',
    lambda document : ReplaceText(
      Find(document.documentElement, 'version'), NEW_VERSION))


def UpdateJavaScript():
  RewriteTextFile('js/package.json',
    lambda line : re.sub(
      r'^  "version": ".*",$',
      '  "version": "%s",' % NEW_VERSION,
      line))


def UpdateMakefile():
  protobuf_version_offset = 11
  expected_major_version = '3'
  if NEW_VERSION_INFO[0] != expected_major_version:
    print """[ERROR] Major protobuf version has changed. Please update
update_version.py to readjust the protobuf_version_offset and
expected_major_version such that the PROTOBUF_VERSION in src/Makefile.am is
always increasing.
    """
    exit(1)

  protobuf_version_info = '%s:%s:0' % (
    int(NEW_VERSION_INFO[1]) + protobuf_version_offset, NEW_VERSION_INFO[2])
  RewriteTextFile('src/Makefile.am',
    lambda line : re.sub(
      r'^PROTOBUF_VERSION = .*$',
      'PROTOBUF_VERSION = %s' % protobuf_version_info,
      line))


def UpdateObjectiveC():
  RewriteTextFile('Protobuf.podspec',
    lambda line : re.sub(
      r"^  s.version  = '.*'$",
      "  s.version  = '%s'" % NEW_VERSION,
      line))


def UpdatePhp():
  def Callback(document):
    def CreateNode(tagname, indent, children):
      elem = document.createElement(tagname)
      indent += 1
      for child in children:
        elem.appendChild(document.createTextNode('\n' + (' ' * indent)))
        elem.appendChild(child)
      indent -= 1
      elem.appendChild(document.createTextNode('\n' + (' ' * indent)))
      return elem

    root = document.documentElement
    version = Find(root, 'version')
    ReplaceText(Find(version, 'release'), NEW_VERSION)
    ReplaceText(Find(version, 'api'), NEW_VERSION)
    now = datetime.datetime.now()
    ReplaceText(Find(root, 'date'), now.strftime('%Y-%m-%d'))
    ReplaceText(Find(root, 'time'), now.strftime('%H:%M:%S'))
    changelog = Find(root, 'changelog')
    for old_version in changelog.getElementsByTagName('version'):
      if Find(old_version, 'release').firstChild.nodeValue == NEW_VERSION:
        print ('[WARNING] Version %s already exists in the change log.'
          % NEW_VERSION)
        return
    changelog.appendChild(document.createTextNode(' '))
    stability = Find(root, 'stability')
    release = CreateNode('release', 2, [
        CreateNode('version', 3, [
          FindAndClone(version, 'release'),
          FindAndClone(version, 'api')
        ]),
        CreateNode('stability', 3, [
          FindAndClone(stability, 'release'),
          FindAndClone(stability, 'api')
        ]),
        FindAndClone(root, 'date'),
        FindAndClone(root, 'time'),
        FindAndClone(root, 'license'),
        FindAndClone(root, 'notes')
      ])
    changelog.appendChild(release)
    changelog.appendChild(document.createTextNode('\n '))
  RewriteXml('php/ext/google/protobuf/package.xml', Callback)

def UpdatePython():
  RewriteTextFile('python/google/protobuf/__init__.py',
    lambda line : re.sub(
      r"^__version__ = '.*'$",
      "__version__ = '%s'" % NEW_VERSION,
      line))

def UpdateRuby():
  RewriteTextFile('ruby/google-protobuf.gemspec',
    lambda line : re.sub(
      r'^  s.version     = ".*"$',
      '  s.version     = "%s"' % NEW_VERSION,
      line))


UpdateConfigure()
UpdateCsharp()
UpdateCpp()
UpdateJava()
UpdateJavaScript()
UpdateMakefile()
UpdateObjectiveC()
UpdatePhp()
UpdatePython()
UpdateRuby()
