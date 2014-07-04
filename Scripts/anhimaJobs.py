#! /usr/bin/env python

import sys
import os
import optparse

parser = optparse.OptionParser()
parser.add_option('--launch', action='store', type='string', dest='launch', help='Launch jobs')
parser.add_option('--sum', action='store', type='string', dest='sum', help='Sum ouput histograms')
(options, args) = parser.parse_args()

curdir = os.getcwd();
sys.path.append(curdir)


if options.launch:
    parameterFile = options.launch
    b = __import__(os.path.splitext(parameterFile)[0]).batch
    b.prepareArea()
    b.launch(simulate=False)
elif options.sum:
    parameterFile = options.sum
    b = __import__(os.path.splitext(parameterFile)[0]).batch
    b.addHistos()
else:
    parser.print_help()
