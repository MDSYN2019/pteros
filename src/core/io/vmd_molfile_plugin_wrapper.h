/*
 * This file is a part of
 *
 * ============================================
 * ###   Pteros molecular modeling library  ###
 * ============================================
 *
 * (C) 2009-2018, Semen Yesylevskyy
 *
 * All works, which use Pteros, should cite the following papers:
 *  
 *  1.  Semen O. Yesylevskyy, "Pteros 2.0: Evolution of the fast parallel
 *      molecular analysis library for C++ and python",
 *      Journal of Computational Chemistry, 2015, 36(19), 1480–1488.
 *      doi: 10.1002/jcc.23943.
 *
 *  2.  Semen O. Yesylevskyy, "Pteros: Fast and easy to use open-source C++
 *      library for molecular analysis",
 *      Journal of Computational Chemistry, 2012, 33(19), 1632–1636.
 *      doi: 10.1002/jcc.22989.
 *
 * This is free software distributed under Artistic License:
 * http://www.opensource.org/licenses/artistic-license-2.0.php
 *
*/


#ifndef VMD_MOLFILE_PLUGIN_WRAPPER_H
#define VMD_MOLFILE_PLUGIN_WRAPPER_H

#include <string>
#include "pteros/core/system.h"
#include "pteros/core/selection.h"
#include "pteros/core/mol_file.h"
#include "molfile_plugin.h"

namespace pteros {

/// Generic API for reading and writing any molecule file formats
class VMD_molfile_plugin_wrapper: public Mol_file {
public:
    // High-level API        
    VMD_molfile_plugin_wrapper(std::string& fname);
    virtual void open(char open_mode);
    virtual ~VMD_molfile_plugin_wrapper();

protected:       
    void* handle; // Handle for reading
    void* w_handle; // Handle for writing

    char mode;

    virtual bool do_read(System *sys, Frame *frame, const Mol_file_content& what);
    virtual void do_write(const Selection &sel, const Mol_file_content& what);

    // molfile plugin instance (set in derived class)
    molfile_plugin_t* plugin;   

    // Static map of all registered plagins.
    // Filled on first load of the library
    static std::map<std::string,molfile_plugin_t*> molfile_plugins;
};

}
#endif /* MOL_FILE_H */

