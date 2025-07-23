# MISRA C++ Static Analysis Report

## Overview

**Static Analysis Tool**: cppcheck with MISRA addon
**Analysis Scope**: Library modules only - src/ include/
**Standard**: MISRA C 2012 (applicable rules for C++)
**Note**: This analysis uses MISRA C 2012 rules as the foundation, which overlap significantly with MISRA C++ 2008.

## Statistics

### Summary

- **Total MISRA Violations**: 11
- **Unique Rules Violated**: 3
- **Files Analyzed**: 3
- **Analysis Errors**: 1

### Violations by Severity

- **Style**: 11

### Violations by Rule

- **MISRA C 2012 Rule 5.8**: 2 violation(s)
- **MISRA C 2012 Rule 8.6**: 4 violation(s)
- **MISRA C 2012 Rule 8.7**: 5 violation(s)

### Violations by File

- **freertos_task.cc**: 4 violation(s)
- **freertos_task.hpp**: 7 violation(s)

### Analyzed Files

- freertos_task.cc
- freertos_task.hpp
- nofile

## Analysis Notes

### MISRA C++ 2008 vs MISRA C 2012

This analysis uses the MISRA C 2012 standard as implemented by cppcheck's MISRA addon. While specifically designed for C, many MISRA C 2012 rules are directly applicable to C++ and overlap with MISRA C++ 2008 requirements:

- **Rule 5.8**: External identifier naming conflicts
- **Rule 8.6**: Function declaration consistency
- **Rule 8.7**: Objects with external linkage
- Additional rules covering similar code quality aspects

### Analysis Limitations

- Rule texts cannot be displayed due to MISRA licensing restrictions
- Some complex template and C++17 constructs may not be fully analyzed
- FreeRTOS-specific configurations may trigger false positives
- This analysis complements the existing clang-tidy static analysis

### Analysis Errors

Some files could not be fully analyzed:

```
/home/runner/work/freertos_cpp_wrappers/freertos_cpp_wrappers/src/freertos_task.cc:0:0: error: Bailing out from analysis: Checking file failed: Failed to execute addon 'misra' - exitcode is 1 [internalError]
```

---
*Generated: July 23, 2025*
*Tool: cppcheck with MISRA addon*
*Scope: Library modules only*
