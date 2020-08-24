#! /usr/bin/env python
###############################################################################
#
# This script is used to generate swig interface file for the given YAML file.
# The generated file will be placed in the directory specified by -o option.
# Assumptions :
#   - The input file is of format <DIR_PATH>/bcm.<module>.api.yaml
#   - The output file would be named as bcm.<module>.i
#
# $Copyright: (c) 2019 Broadcom.
# Broadcom Proprietary and Confidential. All rights reserved.$
#
###############################################################################

from __future__ import unicode_literals
import os
import yaml, sys
import argparse
import collections
import re

parser = argparse.ArgumentParser(description='Generate Interface files')
parser.add_argument('-i', '--input', help="Input YAML file", required=True)
parser.add_argument('-o', '--output', help="Output directory", required=True)
parser.add_argument('-c', '--config', help="Config YAML file")
parser.add_argument('-p', '--param', help="Build parameters file")
parser.add_argument('-d', '--dep', help="Dependency file")
parser.add_argument('-m', '--mk', help="Make fragment")
parser.add_argument('-C', '--cov', help="Coverage file")

args = parser.parse_args(sys.argv[1:])

papi = {}
ifile = args.input
odir = args.output
cfile = args.config
pfile = args.param
dfile = args.dep
mfile = args.mk
cov_file = args.cov
config = {}
param = {}
types = {}

# API coverage
py_bcm_api = {}

file_content = []
dep_content = []
mod_content = []
module = ""
static_cb_decl = 0
typedef_history = {}
entry_type = {}

#
# Standard datatypes for adding into array_class.
#
standard_datatypes = ["int",
                      "uint8",
                      "uint8_t",
                      "uint16",
                      "uint16_t",
                      "uint32",
                      "uint32_t",
                      "uint64",
                      "uint64_t",
                      "int8",
                      "int8_t",
                      "int16",
                      "int16_t",
                      "int32",
                      "int32_t",
                      "int64",
                      "int64_t",
                      "char",
                      ]
scalar_types = []
for dtype in standard_datatypes:
    scalar_types.append(dtype)
    typedef_history[dtype] = dtype

standard_array_types = [
                    "unsigned int",
                    "unsigned short",
                    "unsigned long",
                    "unsigned char",
                    "signed int",
                    "signed short",
                    "signed long",
                    "signed char",
                    "float",
                    "double"
                    ]

#
# Sorted API types
#
defines = []
enums = []
structs = []
typedefs = []
variables = []
funcs = []
enum_list = []

sorted_api_list = [
            enums,
            structs,
            typedefs,
            defines,
            variables,
            funcs
            ]

# Maintain same orfer as sorted_api_list above.
display_heading = [
        "// -----------\n// Enumerations\n// -----------\n",
        "// -----------\n// Structures\n// -----------\n",
        "// -----------\n// Typedefs\n// -----------\n",
        "// -----------\n// Defines\n// -----------\n",
        "// -----------\n// Global Variables\n// -----------\n",
        "// -----------\n// Functions\n// -----------\n"
        ]

# Helper function to convert python unicode to string.
def unicode_repr(self, data):
        return self.represent_str(data.encode('utf-8'))

###############################################################################
# Function : api_name
#
# Description : Helper function to retrieve name of the API.
#               Format : PREFIX + NAME + SUFFIX
#
# Arguments : entry - API info
#
# Returns : Name of the API.
###############################################################################
def api_name(entry, etype):

    if not config:
        return entry['NAME']

    prefix = suffix = ""
    if etype in config['PREFIXES']:
        prefix = config['PREFIXES'][etype]

    if etype in config['SUFFIXES']:
        suffix = config['SUFFIXES'][etype]

    return prefix + entry['NAME'] + suffix

###############################################################################
# Function : is_scalar
#
# Description : Helper function to determine whether the datatype is scalar.
#
# Arguments : param - Parameter data
#
# Returns : TRUE for scalar. FALSE otherwise.
###############################################################################
def is_scalar(param):
    btype = param['BASETYPE']
    ttype = param['TRUETYPE'] if 'TRUETYPE' in param else ""

    if 'PTR' in param or 'ARRAY' in param:
        return False

    if btype in scalar_types or \
            ttype in scalar_types:
            return True

    if types and btype.startswith("bcm_"):
        tname = btype.strip("bcm_")
        if tname.endswith("_t"):
            tname = tname[0:len(tname)-2]
        for entry in types:
            entry_name = entry['NAME'] if 'NAME' in entry else ""
            if tname == entry_name and \
                    'BASETYPE' in entry and \
                    entry['BASETYPE'] in scalar_types and \
                    'PTR' not in entry and 'ARRAY' not in entry:
                        # Add the datatype to scalar list.
                        # This will help next scalar check.
                        scalar_types.append(btype)
                        return True

    return False

###############################################################################
# Function : param_truetype_get
#
# Description : Helper function to get the datatype is scalar param.
#
# Arguments : param - Parameter data
#
# Returns : Type of scalar parameter.
###############################################################################
def param_truetype_get(param):
    btype = param['BASETYPE']
    ttype = param['TRUETYPE'] if 'TRUETYPE' in param else ""

    if ttype in scalar_types:
        return ttype
    if btype in scalar_types:
        return btype

    return None

###############################################################################
# Function : add_func_typemaps
#
# Description : Helper function to handle functions with output aruguments.
#
# Arguments : entry - API info
#
# Returns : None.
###############################################################################
def add_func_typemaps(entry):
    if 'PARAMETERS' not in entry:
        return

    param_content = []
    num_out_args = 0
    out_arg_list = []
    for param in entry['PARAMETERS']:
        if 'DIR' in param and param['DIR'] == "OUT" \
                and is_scalar(param) == False \
                and 'PTR' in param:
            num_out_args += 1
            out_arg_list.append(param['NAME'])

    if num_out_args == 0:
        return

    for pname in out_arg_list:
        arg_str = ""
        for param in entry['PARAMETERS']:
            if param['NAME'] == pname:
                break

        arg_str += param['BASETYPE'] + " "
        num_ptr = 0
        if 'PTR' in param:
            for each_ptr in range(int(param['PTR'])):
                num_ptr += 1
                arg_str += "*"
        else:
            arg_str += "  "

        if num_ptr > 1:
            param_content.append("%typemap(in) " + arg_str + param['NAME'] + " ")
            param_content.append("(" + arg_str[:-1] + " temp0)\n")
            param_content.append("{\n")
            i = 1
            for ptr in range(num_ptr-2):
                param_content.append("\t" + arg_str[:-i] + " temp" + str(i)  + ";\n")
                param_content.append("\ttemp" + str(i)  + " = &temp" + str(i-1)  + ";\n");
                i += 1
            param_content.append("\t$1 = &temp" + str(i-1) + ";\n")
            param_content.append("}\n\n")

    for line in param_content:
        file_content.append(line)

###############################################################################
# Function : format_parameter
#
# Description : Helper function to dump parameters.
#               Used in function and variable processing.
#
# Arguments : param - Parameter data
#             etype - Type of API entry
#
# Returns : None.
###############################################################################
def format_parameter(param, etype, keep_fname=False, skip_fname=False):
    scalar_name = {"IN":"INPUT", "OUT":"OUTPUT", "INOUT":"BIDIR"}
    ptype = param['BASETYPE']
    param_content = []

    # For void type, name is not needed.
    name = ''
    if 'NAME' in param:
        name = param['NAME']

    if 'CONST' in param and param['CONST'] == '1':
        param_content.append("const ")
    param_content.append(ptype + " ")

    # Append * for each PTR
    if 'PTR' in param:
        for each_ptr in range(int(param['PTR'])):
            param_content.append("*")

    # For variables, immaterial of direction, display name.
    if etype == "V" or etype == "SV":
        param_content.append(name)
        return

    # For functions, scalar variable names will depend on direction.
    if skip_fname == False and keep_fname == False and "F" in etype:
        if is_scalar(param):
            # Scalar. Use INPUT, OUTPUT, BIDIR
            param_content.append(scalar_name[param['DIR']])
        else:
            param_content.append(name)
    elif keep_fname == True:
        param_content.append(name)

    if 'ARRAY' in param and param['ARRAY']:
        if param['ARRAY'] == "UNDEF":
            param_content.append("[]")
        else:
            param_content.append("[" + param['ARRAY'] + "]")

    return "".join(param_content)

###############################################################################
# Function : ignore_unresolved_objects
#
# Description : Add objects that are not exported as python callable objects.
#
# Arguments : None.
#
# Returns : None.
###############################################################################
def ignore_unresolved_objects():

    # Following methods cause import error when importing modules in python.
    # This is not needed to be exported to python. So ignore.
    if module in config['header_headers'] and \
            any("shared/port.h" in hdr for hdr in config['header_headers'][module]):
        file_content.append("%ignore _shr_port_config_phy_oam_set;\n")
        file_content.append("%ignore _shr_port_config_phy_oam_get;\n")
        file_content.append("%ignore _shr_port_control_phy_oam_set;\n")
        file_content.append("%ignore _shr_port_control_phy_oam_get;\n")
        file_content.append("%ignore _shr_port_phy_timesync_config_set;\n")
        file_content.append("%ignore _shr_port_phy_timesync_config_get;\n")
        file_content.append("%ignore _shr_port_control_phy_timesync_set;\n")
        file_content.append("%ignore _shr_port_control_phy_timesync_get;\n")
        file_content.append("%ignore _shr_port_control_phy_timesync_enhanced_capture_get;\n")
        file_content.append("\n")

###############################################################################
# Function : add_header
#
# Description : Add header information to the generated interface file.
#
# Arguments : None.
#
# Returns : None.
###############################################################################
def add_header():
   # Add file header.
   # Disclaimer to be added to the generated interface files.
   ifile_name = ifile.rsplit("/").pop()
   file_disclaimer = "###########################################################\n"
   file_disclaimer += "# FILE : bcm." + module + ".i\n"
   file_disclaimer += "# NOTE : Auto-generated from " + ifile_name  +"\n"
   file_disclaimer += "# DO NOT EDIT. All changes will be lost on auto-generation\n"
   file_disclaimer += "# $Copyright: Broadcom Ltd.$\n"
   file_disclaimer += "##########################################################\n"

   file_content.append(file_disclaimer + "\n")
   module_name = module;
   if module in standard_datatypes:
       module_name += "_module"
   file_content.append('%module(package="bcm") ' + module_name + "\n\n")

   file_content.append("// --------\n// Includes\n// --------\n")
   file_content.append("%{\n#define SWIG_FILE_WITH_INIT\n")
   file_content.append('#include <sal/types.h>\n')
   file_content.append('#include <sal/core/libc.h>\n')
   file_content.append('#include <soc/drv.h>\n')
   if config and module in config['header_headers']:
       for hfile in config['header_headers'][module]:
           file_content.append(hfile + '\n')
   file_content.append('#include <bcm/' + module +'.h>\n')
   file_content.append("%}\n")
   file_content.append("\n")

   # Ignore unresolved objects from shared header files.
   ignore_unresolved_objects()

   file_content.append('%include "typemaps.i"\n')
   file_content.append('%include "cpointer.i"\n')
   file_content.append('%include "carrays.i"\n')
   file_content.append('%include "bcm.types.i"\n')

   if config and module in config['header_headers']:
       hfile_excluded = []
       for hfile in config['header_headers'][module]:
           # Manipulate header filename to create interface file name.
           index = hfile.index("<")
           header_file = hfile[index:].strip("<").strip(">")
           if ("types" in header_file and 'shared' not in header_file) or \
                   "bitop" in header_file:
                # Types are already included.
                # CHECK: Issue in swig importing shared/bitop.h file.
                hfile_excluded.append(header_file)
           else:
               if 'shared' in header_file:
                   file_content.append('%include "'+ header_file + '"\n')
               else:
                   file_content.append('%import "'+ header_file + '"\n')
   file_content.append("\n\n")

   # Add array_class for standard data types.
   if module == "types":
       file_content.append("typedef unsigned short  uint16;\n")
       file_content.append("typedef unsigned int  uint32;\n")
       file_content.append("typedef unsigned char  uint8;\n")
       file_content.append("typedef unsigned long  uint64;\n")
       file_content.append("typedef signed short  int16;\n")
       file_content.append("typedef signed int  int32;\n")
       file_content.append("typedef signed char  int8;\n")
       file_content.append("typedef signed long  int64;\n")
       file_content.append("\n")
       for dtype in standard_datatypes:
           dtype_array = dtype.replace(" ", "_") + "_array"
           dtype_ptr = dtype.replace(" ", "_") + "_ptr"
           file_content.append("%array_class(" + dtype +", " + dtype_array + ");\n")
           file_content.append("%pointer_class(" + dtype +", " + dtype_ptr + ");\n")
#       for dtype in standard_array_types:
#           dtype_array = dtype.replace(" ", "_") + "_array"
#           dtype_ptr = dtype.replace(" ", "_") + "_ptr"
#           file_content.append("%array_class(" + dtype +", " + dtype_array + ");\n")
#           file_content.append("%pointer_class(" + dtype +", " + dtype_ptr + ");\n")
       file_content.append("\n\n")


###############################################################################
# Function : handle_defines
#
# Description : Handler function to process defines.
#               Handles API entries of types 'D' and 'SD'.
#
# Arguments : entry - API info
#
# Returns : None.
###############################################################################
def handle_defines(entry):
    global py_bcm_api, entry_type
    i = 1

    if 'DEFINES' not in entry:
        return

    # Get prefix from config file.
    prefix = ""
    if config and entry['TYPE'] == 'D':
        prefix = config['PREFIXES']['D']

    # Process all the defines in the array.
    defines = entry['DEFINES']
    for define in defines:
        if 'NAME' in define and '_NAME' not in define:
            name = define['NAME']
        else:
            name = define
        if name.islower():
            prefix = prefix.lower()
        name = prefix + name

        if 'VALUE' in define:
            value = define['VALUE']
            if value == "~":
                # CHECK : Find why ~ comes in yml file.
                # The value matches the generated header files.
                value = ("0x%.8x" % i)
        else:
            value = ("0x%.8x" % i)

        if 'PARAMETERS' not in define:
            file_content.append("#define %-30s  %s\n" %
                    (name , str(value)))
            i *= 2
            continue

        # For each parameter, form the string.
        param_wo_basetype = "("
        param_w_basetype = "("

        plist = []
        param_list = []
        param_id = 1

        # Resolve return type
        if 'RETURNS' in define and 'BASETYPE' in define['RETURNS']:
            returns = define['RETURNS']['BASETYPE']
            if 'PTR' in define['RETURNS']:
                for each_ptr in range(int(define['RETURNS']['PTR'])):
                    returns += "*"
        else:
            returns = "void"

        params = define['PARAMETERS']
        prev = 0
        for param in params:
            if prev == 1:
                param_w_basetype += ", "
                param_wo_basetype += ", "
            prev = 1

            # Ignore functional macros without basetype.
            if 'BASETYPE' not in param:
                print ("Note: No basetype in functional macro " + name[:-2] + ". Ignored\t")
                return
            param_w_basetype += param['BASETYPE'] + " "
            param_list.append(param)
            param_id += 1

            # Get number of pointers
            num_ptr = 0
            if 'PTR' in param:
                for each_ptr in range(int(param['PTR'])):
                    num_ptr += 1
                    param_w_basetype += "*"

            # Get the parameter name
            param_wo_basetype += param['NAME']
            param_w_basetype += param['NAME']
            plist.append(param['NAME'])

        param_wo_basetype += ")"
        param_w_basetype += ")"

        # Form the macro declaration and fake definition for SWIG.
        func_macro_orig = name + param_wo_basetype
        func_macro_defn = returns + " " + name + param_w_basetype

        # WAR : Below macros cause compilation error. Adjust value.
        if name == "BCM_FABRIC_GROUP_MODID_SET" or \
                name == "BCM_FABRIC_LOCAL_MODID_SET":
            value = value.split("=")[1]

        # Add C define for generated wrapper file.
        define_str = "#define %-30s  %s\n" % (func_macro_orig , str(value))
        file_content.append("%{\n" + define_str + "%}\n")

        # Add typemap to append arguments to output.
        typemap_info = ""
        typemap_info += "%typemap(argout) " + param_w_basetype + " {\n"
        for pid in range(int(param_id)):
            if pid == 0:
                continue
            btype = param_truetype_get(param_list[pid - 1])
            if btype != None or ('BASETYPE' in param_list[pid - 1] and param_list[pid - 1]['BASETYPE'] in enum_list):
                swig_func = argout_symbol.get(btype, "SWIG_From_int")
                ptype = py_type.get(btype, "int")
                typemap_info += "\tPyObject *out" + str(pid) + " = " + swig_func + "((" +ptype +")($" + str(pid) + "));\n"
            elif 'PTR' in param_list[pid - 1] or 'ARRAY' in param_list[pid - 1]:
                typemap_info += "\tPyObject *out" + str(pid)+ " =  SWIG_NewPointerObj(SWIG_as_voidptr($" + str(pid)+ "), $" + str(pid)+ "_descriptor, 0);\n"
            else:
                typemap_info += "\t$" + str(pid)+ "_ltype *parg" + str(pid) + ";\n"
                typemap_info += "\tparg" + str(pid) + " = malloc(sizeof($" + str(pid)+ "_ltype));\n"
                typemap_info += "\tmemcpy(parg" + str(pid) + ", &$1, sizeof($" + str(pid) +  "_ltype));\n"
                typemap_info += "\tPyObject *out" + str(pid)+ " =  SWIG_NewPointerObj(SWIG_as_voidptr(parg" + str(pid)+ "), $&" + str(pid)+ "_descriptor, 0);\n"
            typemap_info += "\t$result = SWIG_Python_AppendOutput ($result, out" + str(pid) + ");\n"
        typemap_info += "}\n"
        file_content.append(typemap_info)

        # Fake declaration to generate wrapper for macro.
        file_content.append(func_macro_defn + ";\n")

        # Clear the typemaps.
        typemap_info = "%typemap(argout) " + param_w_basetype + ";\n"
        file_content.append(typemap_info)

        # Add to coverage.
        py_bcm_api[module]['DEFINES'].append(name)

###############################################################################
# Function : handle_structs
#
# Description : Handler function to process structs and typedef structs.
#               Handles API entries of types 'S' and 'TS'.
#
# Arguments : entry - API info
#
# Returns : None.
###############################################################################
def handle_structs(entry):
    global py_bcm_api, entry_type
    etype = entry['TYPE']
    name = api_name(entry, etype)
    entry_type[name] = entry['TYPE'];

    # Add entry header.
    if etype == 'TS':
        name = api_name(entry, "S")
        file_content.append("typedef ")
    file_content.append("struct " + name + " {\n")

    # Process parameters.
    if 'PARAMETERS' in entry:
        for param in entry['PARAMETERS']:
            ptype = param['BASETYPE']
            pname = param['NAME']
            parray = ""
            if 'ARRAY' in param and param['ARRAY']:
                if param['ARRAY'] == "UNDEF":
                    parray ="[]"
                else:
                    parray ="[" + param['ARRAY'] + "]"

            pptr = ""
            if 'PTR' in param:
                for each_ptr in range(int(param['PTR'])):
                    pptr += "*"

            file_content.append("    " + ptype + " " + \
                    pptr + pname + parray +";\n")

    # Add entry footer.
    if etype == 'TS':
        name = api_name(entry, etype)
        file_content.append("} " + name + ";\n")
        file_content.append("%array_class(" + name + ", " + name + "_array);\n")
        file_content.append("%pointer_class(" + name + ", " + name + "_ptr);\n")
    else:
        file_content.append("};\n")

    # Add to coverage.
    py_bcm_api[module]['STRUCTS'].append(name)


###############################################################################
# Function : handle_variables
#
# Description : Handler function to process variables.
#               Handles API entries of types 'V' and 'SV'.
#
# Arguments : entry - API info
#
# Returns : None.
###############################################################################
def handle_variables(entry):
    global py_bcm_api, entry_type
    etype = entry['TYPE']
    name = api_name(entry, etype)
    entry_type[name] = entry['TYPE'];
    btype = entry['BASETYPE']

    file_content.append("extern " + btype + " ")
    if 'FUNC' in entry and entry['FUNC'] == '1':
        file_content.append("(* " + name + ")")
        file_content.append("(")
        if 'PARAMETERS' in entry:
            prev = 0
            for param in entry['PARAMETERS']:
                if prev == 1:
                    file_content.append(", ")
                prev = 1
                file_content.append(format_parameter(param, etype))
        else:
            file_content.append("void")
        file_content.append(")")
        if 'COMP_ATTR' in entry:
            file_content.append("\n   " + entry['COMP_ATTR'])

    else:
        # Append * for each PTR
        if 'PTR' in entry:
            for each_ptr in range(int(entry['PTR'])):
                file_content.append("*")

        file_content.append(name)
        if 'ARRAY' in entry and entry['ARRAY']:
            if entry['ARRAY'] == "UNDEF":
                file_content.append("[]")
            else:
                file_content.append("[" + entry['ARRAY'] + "]")

    file_content.append(";\n")

    # Add to coverage.
    py_bcm_api[module]['VARIABLES'].append(name)

# Following typedefs are resolved in the shared include files.
# This is hidden from PAPI.
skip_py_type_gen = [
                "bcm_dma_chan_t",
                "bcm_fabric_predicate_vector_t",
                "bcm_port_abil_t",
                "bcm_port_ms_t",
                "bcm_port_if_t",
                "bcm_port_duplex_t",
                "bcm_port_phy_reset_cb_t",
                "bcm_port_mdix_t",
                "bcm_port_mdix_status_t",
                "bcm_port_medium_t",
                "bcm_port_medium_status_cb_t",
                "bcm_phy_config_t",
                "bcm_port_mcast_flood_t",
                "bcm_port_encap_t",
                "bcm_pa_encap_t",
                "bcm_port_ethertype_t",
                "bcm_port_prbs_t",
                "bcm_port_phy_pcs_mode_t",
                "bcm_port_cable_state_t",
                "bcm_port_cable_diag_t",
                "bcm_port_phy_control_t",
                "bcm_port_stp_t",
        ]

###############################################################################
# Function : handle_typedefs
#
# Description : Handler function to process typedefs.
#               Handles API entries of types 'T', 'ST' and 'CT'.
#
# Arguments : entry - API info
#
# Returns : None.
###############################################################################
def handle_typedefs(entry):
    global py_bcm_api, entry_type
    etype = entry['TYPE']

    # Retrieve API name
    if etype == "T":
        name = api_name(entry, etype)
    else:
        name = entry['NAME']
    entry_type[name] = entry['TYPE'];

    # Generate interaface file data.
    basetype = entry['BASETYPE']
    file_content.append("typedef " + basetype + " " + name)

    # Handle array.
    array = False
    if 'ARRAY' in entry and entry['ARRAY']:
        if entry['ARRAY'] == "UNDEF":
            file_content.append("[]")
        else:
            file_content.append("[" + entry['ARRAY'] + "]")
            array = True

    file_content.append(";\n")

    while True:
        if basetype in standard_datatypes:
            typedef_history[name] = basetype;
            if 'PTR' not in entry and 'ARRAY' not in entry:
                scalar_types.append(name)
            break

        bt = basetype
        while bt in typedef_history:
            bt = typedef_history[bt]
            if bt in standard_datatypes:
                typedef_history[name] = bt
                if 'PTR' not in entry and 'ARRAY' not in entry:
                    scalar_types.append(name)
                break
        break

    # Add python class.
    if array == True and name not in skip_py_type_gen:
        file_content.append("%pythoncode %{\n")
        file_content.append("class " + name + "(" + basetype + "_array):\n")
        file_content.append("\tdef __init__(self):\n")
        file_content.append("\t\t" + basetype + "_array.__init__(self, " + entry['ARRAY'] + ")\n")
        file_content.append("%}\n")
    else:
        if (basetype not in standard_datatypes and basetype not in enum_list) and \
                name not in typedef_history and \
                name not in skip_py_type_gen:
            file_content.append("%pythoncode %{\n")
            file_content.append("class " + name + "(" + basetype + "):\n")
            file_content.append("\tdef __init__(self):\n")
            file_content.append("\t\t" + basetype + ".__init__(self)\n")
            file_content.append("%}\n")

    # Add to coverage.
    py_bcm_api[module]['TYPEDEFS'].append(name)


###############################################################################
# Function : handle_funcs
#
# Description : Handler function to process function definitions.
#               Handles API entries of types 'F' and 'SF'.
#
# Arguments : entry - API info
#
# Returns : None.
###############################################################################
def handle_funcs(entry):
    global py_bcm_api, entry_type
    etype = entry['TYPE']
    name = api_name(entry, etype)
    entry_type[name] = entry['TYPE'];

    add_func_typemaps(entry)
    decl_str = ""
    cb_str = ""

    decl_str += "extern "
    if 'RETURNS' in entry:
        decl_str += entry['RETURNS']['BASETYPE'] + " "
        cb_str += entry['RETURNS']['BASETYPE'] + " "
    else:
        decl_str += "void "
        cb_str += "void "
    decl_str += name + " (\n"
    cb_str += name + " (\n"

    if 'PARAMETERS' in entry:
        prev = 0
        for param in entry['PARAMETERS']:
            if prev == 1:
                decl_str += ",\n"
                cb_str += ", "
            prev = 1
            decl_str += "    "
            decl_str += format_parameter(param, etype)
            cb_str += "    " + format_parameter(param, etype, skip_fname=True)
    else:
        decl_str += "void"
        cb_str += "void"
    decl_str += ")"
    cb_str += ");\n"

    if 'COMP_ATTR' in entry:
        decl_str += "\n   " + entry['COMP_ATTR']
    decl_str += ";\n"

    if 'DOC' in entry:
        docstring = entry['DOC'].__repr__().replace('"', '').replace("'", "")
        file_content.append('%feature("autodoc", "'+ docstring + '");\n')
    file_content.append('%callback("%s_cb");\n')
    file_content.append(cb_str)
    file_content.append('%nocallback;\n')
    file_content.append(decl_str)
    if 'DOC' in entry:
        file_content.append('%feature("autodoc", "");\n')

    # Add to coverage.
    py_bcm_api[module]['FUNCTIONS'].append(name)

# Symbol of argument type used by Py_BuildValue()
arg_symbol = {
    "int8"      : "PyInt_FromLong",
    "int8_t"    : "PyInt_FromLong",
    "char"      : "PyInt_FromLong",
    "uint8"     : "PyLong_FromUnsignedLong",
    "uint8_t"   : "PyLong_FromUnsignedLong",
    "int16"     : "PyInt_FromLong",
    "int16_t"   : "PyInt_FromLong",
    "uint16"    : "PyLong_FromUnsignedLong",
    "uint16_t"  : "PyLong_FromUnsignedLong",
    "int"       : "PyInt_FromLong",
    "int32"     : "PyInt_FromLong",
    "int32_t"   : "PyInt_FromLong",
    "uint32"    : "PyLong_FromUnsignedLong",
    "uint32_t"  : "PyLong_FromUnsignedLong",
    "int64"     : "PyLong_FromLongLong",
    "int64_t"   : "PyLong_FromLongLong",
    "uint64"    : "PyLong_FromUnsignedLongLong",
    "uint64_t"  : "PyLong_FromUnsignedLongLong",
}

argout_symbol = {
    "int8"      : "SWIG_From_signed_SS_char",
    "int8_t"    : "SWIG_From_signed_SS_char",
    "char"      : "SWIG_From_signed_SS_char",
    "uint8"     : "SWIG_From_unsigned_SS_char",
    "uint8_t"   : "SWIG_From_unsigned_SS_char",
    "int16"     : "SWIG_From_short",
    "int16_t"   : "SWIG_From_short",
    "uint16"    : "SWIG_From_unsigned_SS_short",
    "uint16_t"  : "SWIG_From_unsigned_SS_short",
    "int"       : "SWIG_From_int",
    "int32"     : "SWIG_From_int",
    "int32_t"   : "SWIG_From_int",
    "uint32"    : "SWIG_From_unsigned_SS_int",
    "uint32_t"  : "SWIG_From_unsigned_SS_int",
    "int64"     : "SWIG_From_unsigned_SS_long",
    "int64_t"   : "SWIG_From_unsigned_SS_long",
    "uint64"    : "SWIG_From_unsigned_SS_long",
    "uint64_t"  : "SWIG_From_unsigned_SS_long",
}

py_type = {
    "int8"      : "SWIG_From_signed_SS_char",
    "int8_t"    : "SWIG_From_signed_SS_char",
    "char"      : "SWIG_From_signed_SS_char",
    "uint8"     : "unsigned char",
    "uint8_t"   : "unsigned char",
    "int16"     : "short",
    "int16_t"   : "short",
    "uint16"    : "unsigned short",
    "uint16_t"  : "unsigned short",
    "int"       : "int",
    "int32"     : "int",
    "int32_t"   : "int",
    "uint32"    : "unsigned int",
    "uint32_t"  : "unsigned int",
    "int64"     : "unsigned long",
    "int64_t"   : "unsigned long",
    "uint64"    : "unsigned long",
    "uint64_t"  : "unsigned long",
}

type_terminator = {
    "S"  : "_s",
    "TS" : "_s",
    "E"  : "_e",
    "TE" : "_e",
    "STE": "_e",
}

###############################################################################
# Function : handle_funcs
#
# Description : Handler function to process typedef function definitions.
#               Handles API entries of types 'TF' and 'STF'.
#
# Arguments : entry - API info
#
# Returns : None.
###############################################################################
def handle_tfuncs(entry):
    global py_bcm_api, entry_type
    global static_cb_decl
    etype = entry['TYPE']
    name = api_name(entry, etype)
    entry_type[name] = entry['TYPE'];

    file_content.append("typedef ")
    if 'BASETYPE' in entry:
        file_content.append(entry['BASETYPE'] + " ")
        fret = entry['BASETYPE']
    elif 'RETURNS' in entry:
        if 'BASETYPE' in entry['RETURNS']:
            file_content.append(entry['RETURNS']['BASETYPE'] + " ")
            fret = entry['RETURNS']['BASETYPE']
        else:
            file_content.append(entry['RETURNS'] + " ")
            fret = entry['RETURNS']
    else:
        file_content.append("void ")
        fret = "void"
    file_content.append("(*" + name + ")(\n")

    param_content = []

    # Below variables are used for constructing callback function.
    arg_w_type_list = ""
    if 'PARAMETERS' in entry:
        prev = 0
        for param in entry['PARAMETERS']:
            if prev == 1:
                param_content.append(",\n")
                arg_w_type_list += ",\n"
            prev = 1
            param_content.append("    ")
            param_content.append(format_parameter(param, etype))
            arg_w_type_list += "    "
            arg_w_type_list += format_parameter(param, etype, keep_fname=True)
    else:
        param_content.append("void")

    for line in param_content:
        file_content.append(line)

    file_content.append(");\n\n")

    # Python callback register function wrapper
    file_content.append("%{\n")
    cb_var_name = "bcm_py_" + name + "_cb"
    file_content.append("static PyObject *" + cb_var_name + " = NULL;\n\n")

    # Python callback dispatch function wrapper
    file_content.append("//Dispatch callback function\n")
    file_content.append(fret +" bcm_py_" + name + "_dispatch(\n")
    file_content.append(arg_w_type_list)
    file_content.append(")\n{\n")
    if fret != "void":
        file_content.append("\tint ret = -1;\n\n");
    file_content.append("\tSWIG_PYTHON_THREAD_BEGIN_ALLOW;\n\n")
    file_content.append("\tif (" + cb_var_name +" != NULL) {\n")
    py_obj_available = {}
    if 'PARAMETERS' in entry:
        for param in entry['PARAMETERS']:
            py_obj_available[param['NAME']] = False
            if is_scalar(param):
                continue
            if 'PTR' in param and param['BASETYPE'] != "void":
                ptrs = ""
                if 'PTR' in param:
                    for each_ptr in range(int(param['PTR'])):
                        ptrs += "p_"

                try:
                    btype = typedef_history[param['BASETYPE']] + "_ptr"
                except:
                    term = type_terminator.get(entry_type[param['BASETYPE']], "")
                    btype = re.sub("_t$", term, param['BASETYPE'])
                param_defn = "\t\tPyObject *p_" + param['NAME']  + " = "
                param_defn += "SWIG_NewPointerObj(SWIG_as_voidptr(" + param['NAME'] + "),"
                param_defn += " SWIGTYPE_" + ptrs  + btype +", 0);\n"
                file_content.append(param_defn)
                py_obj_available[param['NAME']] = True

    file_content.append("\t\tPyObject *result = ")
    file_content.append("PyObject_CallFunctionObjArgs(" + cb_var_name +",\n")
    prefix = "\t\t\t\t\t\t\t\t\t\t\t  "
    if 'PARAMETERS' in entry:
        for param in entry['PARAMETERS']:
            if py_obj_available[param['NAME']] == True:
                file_content.append(prefix + "p_" + param['NAME'] + ",\n")
            elif 'PTR' in param:
                file_content.append(prefix + param['NAME'] + ",\n")
            elif is_scalar(param):
                try:
                    btype = typedef_history[param['BASETYPE']]
                except:
                    btype = param['BASETYPE'];
                py_arg_prefix = arg_symbol.get(btype, "PyInt_FromLong")
                file_content.append(prefix + py_arg_prefix + "(" + param['NAME'] + "),\n")
            else:
                file_content.append(prefix + param['NAME'] + ",\n")
    file_content.append(prefix + "NULL);\n")
    if fret != "void":
        file_content.append("\t\tret = PyInt_AsLong(result);\n")
    if 'PARAMETERS' in entry:
        for param in entry['PARAMETERS']:
            if py_obj_available[param['NAME']] == True:
                file_content.append("\t\tPy_XDECREF(" + "p_" + param['NAME']  + ");\n")
    file_content.append("\t\tPy_XDECREF(result);\n\n")
    file_content.append("\t}\n\n")
    file_content.append("\tSWIG_PYTHON_THREAD_END_ALLOW;\n\n")
    if fret == "void":
        file_content.append("\treturn;\n")
    else:
        file_content.append("\treturn ret;\n")
    file_content.append("}\n\n")

    file_content.append("//Register the callback function\n")
    file_content.append(name + " bcm_py_" + name + "_register")
    file_content.append("(PyObject *callbackFunc)\n{\n")
    file_content.append("\tSWIG_PYTHON_THREAD_BEGIN_ALLOW;\n\n")
    file_content.append("\tif ((callbackFunc != 0) && (callbackFunc != Py_None)) {\n")
    file_content.append("\t\tPy_XDECREF(" + cb_var_name +");\n")
    file_content.append("\t\tPy_XINCREF(callbackFunc);\n")
    file_content.append("\t\t" + cb_var_name + " = callbackFunc;\n")
    file_content.append("\t}\n\n")
    file_content.append("\tSWIG_PYTHON_THREAD_END_ALLOW;\n")
    file_content.append("\treturn bcm_py_" + name + "_dispatch;\n")
    file_content.append("}\n\n")

    # Python callback unregister function wrapper
    file_content.append("//Unregister the callback function\n")
    file_content.append("void bcm_py_" + name + "_unregister")
    file_content.append("(PyObject *callbackFunc)\n{\n")
    file_content.append("\tSWIG_PYTHON_THREAD_BEGIN_ALLOW;\n\n")
    file_content.append("\tif (callbackFunc == " + cb_var_name + ") {\n")
    file_content.append("\t\tPy_XDECREF(" + cb_var_name + ");\n")
    file_content.append("\t\t" + cb_var_name + " = NULL;\n")
    file_content.append("\t}\n\n")
    file_content.append("\tSWIG_PYTHON_THREAD_END_ALLOW;\n")
    file_content.append("}\n\n")

    file_content.append("%}\n\n")
    
    # Add typemaps.
    file_content.append("%typemap(in) " + name  + " {\n")
    file_content.append("\tif (!PyCallable_Check($input)) SWIG_fail;\n")
    file_content.append("\t$1 = bcm_py_" + name + "_register($input);\n}\n\n")
    file_content.append("\t%typemap(out) " + name  + " {\n")
    file_content.append("$result = SWIG_Py_Void();\n}\n\n")

    if static_cb_decl == 0:
        static_cb_decl = 1
        file_content.append("%typemap(in) void * {\n")
        file_content.append("\t$1 = $input;\n}\n\n")
        file_content.append("%typemap(out) void * {\n")
        file_content.append("\t$result = $1;\n}\n\n")

    # Declaration for register & unregister functions.
    file_content.append(name + " bcm_py_" + name + "_register")
    file_content.append("(PyObject *callbackFunc);\n")
    file_content.append("void bcm_py_" + name + "_unregister")
    file_content.append("(PyObject *callbackFunc);\n")
    file_content.append(fret + " bcm_py_" + name + "_dispatch(\n")
    file_content.append(arg_w_type_list)
    file_content.append(");\n")
    file_content.append("\n")

    # Add to coverage.
    py_bcm_api[module]['FUNCTIONS'].append(name)

###############################################################################
# Function : handle_enums
#
# Description : Handler function to process enums.
#               Handles API entries of types 'E', 'TE' and 'STE'.
#
# Arguments : entry - API info
#
# Returns : None.
###############################################################################
def handle_enums(entry):
    global py_bcm_api, entry_type
    global enum_list

    etype = entry['TYPE']
    name = api_name(entry, etype)
    entry_type[name] = entry['TYPE'];

    # Add entry header.
    if etype != 'E':
        name = api_name(entry, "E")
        file_content.append("typedef ")
    file_content.append("enum " + name + "{\n")

    # Add prefix.
    prefix = ""
    if etype != "STE":
        prefix = "bcm"

    # Process parameters.
    prev = 0
    for param in entry['PARAMETERS']:
        if prev == 1:
            file_content.append(",\n")
        prev = 1

        if 'VALUE' in param:
            pvalue = param['VALUE']
            pname = prefix + param['NAME']
            file_content.append("    " + pname + " = " + pvalue)
        elif 'NAME' in param:
            pname = prefix + param['NAME']
            file_content.append("    " + pname)
        else:
            file_content.append("    " + prefix + param)

    # Add entry footer.
    if etype != 'E':
        name = api_name(entry, etype)
        file_content.append("\n} " + name + ";\n")
    else:
        file_content.append("\n};\n")

    enum_list.append(name)

    # Add to coverage.
    py_bcm_api[module]['ENUMS'].append(name)

###############################################################################
# Function : handle_error
#
# Description : Error handler.
#
# Arguments : entry - API info
#
# Returns : None.
###############################################################################
def handle_error(entry):
    raise ValueError("Invalid API type '" + entry['TYPE'] + "'.")

#
# List of handlers based on the API entry type.
#
handler = {
    "D"  : handle_defines,
    "SD" : handle_defines,
    "S"  : handle_structs,
    "TS" : handle_structs,
    "E"  : handle_enums,
    "TE" : handle_enums,
    "STE": handle_enums,
    "V"  : handle_variables,
    "SV" : handle_variables,
    "T"  : handle_typedefs,
    "ST" : handle_typedefs,
    "CT" : handle_typedefs,
    "F"  : handle_funcs,
    "SF" : handle_funcs,
    "TF" : handle_tfuncs,
    "STF": handle_tfuncs,
}

sort_api = {
    "D"  : defines,
    "SD" : defines,
    "S"  : structs,
    "TS" : structs,
    "E"  : enums,
    "TE" : enums,
    "STE": enums,
    "V"  : variables,
    "SV" : variables,
    "T"  : typedefs,
    "ST" : typedefs,
    "CT" : typedefs,
    "F"  : funcs,
    "SF" : funcs,
    "TF" : funcs,
    "STF": funcs,
}

###############################################################################
# Function : yaml_load
#
# Description : Load YAML file and process exceptions.
#
# Arguments : yaml_file - yaml file name
#
# Returns : yaml data.
###############################################################################
def yaml_load(yaml_file):
    with open(yaml_file, 'r') as stream:
        try:
            return yaml.load(stream, yaml.loader.BaseLoader)
        except yaml.YAMLError as exception:
            print(exception)


###############################################################################
# Function : check_feature
#
# Description : Function to check whether the (optional) feature is enabled.
#               Features like port are enabled by default.
#               For conditional features, check whether it is enabled in
#               FEATURE_LIST.
#               For example, Check for INCLUDE_L3 in CFLAGS.
#
# Arguments : None.
#
# Returns : True if feature is enabled, False otherwise.
###############################################################################
def check_feature(condition):

    # Config file is not available to check. So return TRUE.
    if not config:
        return True

    # Add similar check for CFLAGS.
    if 'CFGFLAGS' not in os.environ:
        return True

    if not condition:
        # Default modules would not be in config['CONDITION'].
        if module not in config['CONDITION']:
            return True
        condition = config['CONDITION'][module]
        # condition looks like --> defined(INCLUDE_L3)

    cflags = os.environ['CFGFLAGS']
    if condition.rsplit("(")[1].strip(")") in cflags:
        return True

    return False

###############################################################################
# Function : _generate
#
# Description : Main function to generate the interface file.
#               Handler functions would be invoked for each API
#               based on the API type.
#               The file contents collated from each of the handlers
#               would be written to the interface file.
#
# Arguments : None.
#
# Returns : None.
###############################################################################
def _generate():
    global config, param, types, module, file_content, static_cb_decl
    global py_bcm_api
    module_list = []
    blacklist = {}

    # Load YAML files.
    api_list = collections.OrderedDict()
    api_list = yaml_load(ifile)

    if cfile:
        config = yaml_load(cfile)

    if pfile:
        param = yaml_load(pfile)
        for mod in param['blacklist']:
            print('Blacklisting ' + mod)
            blacklist[mod.upper()] = True

    mod_api_list = {}

    # Handle types and port modules first.
    # The datatypes are internally resolved for callback functions.
    # So need visibility to basic datatypes.
    module_list.append("TYPES")
    mod_api_list["TYPES"] = []
    module_list.append("PORT")
    mod_api_list["PORT"] = []

    for entry in api_list:
        if 'MODULE' in entry:
            mod = entry['MODULE'].upper()
            if mod not in blacklist:
                if mod not in mod_api_list:
                    module_list.append(mod);
                    mod_api_list[mod] = []
                mod_api_list[mod].append(entry)

    # Save types for later use.
    if 'TYPES' in mod_api_list:
        types = mod_api_list['TYPES']

    # For each entry in the API list, call respective handler.
    for mod in module_list:
        static_cb_decl = 0
        entry_list = mod_api_list[mod]

        for entry_type in sorted_api_list:
            del entry_type[:]
        del file_content[:]

        entry = entry_list[0]
        module = entry['MODULE']
        ofile = odir + "/bcm." + module +".i"

        # Intialize coverage.
        py_bcm_api[module] = {}
        py_bcm_api[module]['DEFINES'] = []
        py_bcm_api[module]['STRUCTS'] = []
        py_bcm_api[module]['FUNCTIONS'] = []
        py_bcm_api[module]['VARIABLES'] = []
        py_bcm_api[module]['ENUMS'] = []
        py_bcm_api[module]['TYPEDEFS'] = []

        # Check whether the feature is enabled.
        if not check_feature(""):
            continue

        # Sort the entries as per API type.
        for entry in entry_list:
            # Add interface file header.
            sort_api.get(entry['TYPE'], handle_error).append(entry)

        # Get module name and output file name.
        add_header()

        # Process sorted entries
        idx = 0
        for api_sublist in sorted_api_list:
            if len(api_sublist):
                file_content.append(display_heading[idx]);
            for entry in api_sublist:
                # print entry
                if 'CONDITION' in entry:
                    file_content.append("#if " + entry['CONDITION']  + "\n")
                handler.get(entry['TYPE'], handle_error)(entry)
                if 'CONDITION' in entry:
                    file_content.append("#endif\n")
                file_content.append("\n")
            file_content.append("\n")
            idx += 1

        fh = open(ofile, "w")
        fh.writelines(file_content)
        fh.close()
        py_file = "$(BCM_PY_DIR)/"+module+".py"
        i_file = "$(BCM_PY_DIR)/bcm."+module+".i"
        mod_content.append("PY_FILES += "+py_file+"\n")
        dep_content.append(py_file+": "+i_file+"\n")
        #dep_content.append("\t$(SWIG) -v -I$(SDK)/include -python $(CFGFLAGS) $<\n\n")
    # dependency file
    fh = open(mfile, "w")
    fh.writelines(mod_content)
    fh.close()
    fh = open(dfile, "w")
    fh.writelines(dep_content)
    fh.close()

    if cov_file != None:
        yaml.representer.Representer.add_representer(unicode, unicode_repr)
        fh = open(cov_file,"wb+")
        yaml.dump(py_bcm_api, fh, allow_unicode=True, default_flow_style=False)
        fh.close()

_generate()
