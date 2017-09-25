#!/usr/bin/env python3

from pteros import *
import sys, pkgutil, copy, os, imp, signal
from inspect import getmembers, isclass

import pteros_analysis_plugins


#--------------------------------------
def general_help():
    print("""Usage:
pteros_analysis.py -f <files> <processing options>... -task name1 <task1 options> -task name2 <task2 options> ...

-help traj
    Help for trajectory processing options
-help plugins
    List available analysis plugins
-help <plugin name>
    Detailed help for particular analysis plugin
-help all
    Detailed help for all analysis plugins and trajectory processing options
""")
#--------------------------------------

if __name__ == '__main__':        
    print("+--------------------------------+")
    print("+ This is pteros_analysis script +")
    print("+--------------------------------+")

    # Create logger
    log = Logger('analysis')

    # Check if we have at least one command-line argument:
    if len(sys.argv)==1:
        # Show usage message
        general_help()
        sys.exit(0)

    # Dirty hack - set special SIGINT handler, which kills the whole program
    # instead of raising KeyboardInterrupt in Python.
    # This allows to kill the process if it runs heavy compiled extension at background
    signal.signal(signal.SIGINT, signal.SIG_DFL)

    # Parse command line
    opt,task_opts = parse_command_line(sys.argv,"task")

    if len(task_opts)==0:        
        general_help()
        sys.exit(0)

    # Create trajectory reader
    reader = Trajectory_reader(opt)

    # Container for tasks
    python_tasks = []

    # If explicitly asked for help show it
    if opt.has("help"):
        topic = opt("help","all").as_string()
        if topic == "traj":
            # Show trajectory processing options
            print( reader.help() )

    # Load all supplied tasks
    files_to_load = set()

    log.info("Requested tasks:")
    for task in task_opts:
        f = task.get_name()
        log.info("\t%s" % task.get_name())
        if f not in files_to_load:
            files_to_load.add(f)

    task_num = 0
    log.info("Creating task instances:")
    for f in files_to_load:
        # If file ends explicitly by .py then it is custom plugin
        if f.split('.')[-1] == "py":
            # Get full path
            full = os.path.abspath(f)
            # extract module name
            (mod_name,ext) = os.path.splitext(os.path.basename(full))
            # Append module search path
            sys.path.append(os.path.dirname(full))
            log.info("\tLoading custom plugin '%s'" % f)
            module = __import__(mod_name, fromlist="dummy")
            # See what is inside
            class_list = [o for o in getmembers(module) if (isclass(o[1]) and not ('pteros' in str(o[1]))) ]
            # Check parents of found classes
            nparent = 0
            for c in class_list:
                for base in c[1].__bases__:
                    if 'Task_base' in str(base):
                       nparent+=1
                       class_name = c[0]

            if nparent!=1:
                log.error('Plugin file must contain exactly one class derived from Plugin_base!')
                print(class_list)
                exit()

        else:
            # Seems to be plugin from standard location
            log.info("\tLoading standard plugin '%s'" % f)
            module = __import__(pteros_analysis_plugins.__name__ + "." + f, fromlist="dummy")            
            class_name = f

        # Create needed number of task instances
        for task in task_opts:
            if task.get_name() == f:
                # create new instance of task from that module
                class_ = getattr(module, class_name) # Get class by name
                obj = class_(task) # create instance
                # Workaround to set the correct class name in logger
                if f.split('.')[-1] == "py":
                    obj._class_name = obj.__class__.__name__
                task_num += 1
                reader.add_task( obj )
                log.info('\t\tCreated instance of task {}'.format(class_name))
                if f.split('.')[-1] == "py":
                    # For pure python plugins
                    python_tasks.append(obj) # Save it


    #--------------------------------------

    # RUN!
    reader.run()    




