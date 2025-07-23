#!/usr/bin/env python3
"""
MISRA C 2012 Rule Descriptions Database

This module contains publicly available descriptions for MISRA C 2012 rules.
The descriptions are derived from open sources and documentation to help
developers understand the purpose and rationale behind each rule.

Note: These descriptions are educational summaries based on publicly available
information about MISRA C 2012 rules and are not a replacement for the
official MISRA C 2012 standard.
"""

# MISRA C 2012 Rule Descriptions
# Source: Publicly available documentation and educational materials
MISRA_RULE_DESCRIPTIONS = {
    "2.3": {
        "title": "A project should not contain unused type declarations",
        "description": "Unused type declarations may indicate incomplete code or can be removed to simplify the codebase. This rule helps identify dead code and improves maintainability.",
        "rationale": "Unused declarations can confuse developers and increase compilation time unnecessarily."
    },
    
    "2.7": {
        "title": "There should be no unused parameters in functions",
        "description": "Function parameters that are not used may indicate an error in the implementation or can be removed to clarify the interface. If a parameter must remain for interface compatibility, it should be explicitly marked as unused.",
        "rationale": "Unused parameters can indicate incomplete implementations or interface issues and should be explicitly handled."
    },
    
    "3.1": {
        "title": "The character sequences /* and // shall not appear in /* comment */",
        "description": "Comment delimiters should not appear within comments to avoid confusion about comment boundaries and prevent accidental code commenting.",
        "rationale": "Nested comment sequences can lead to unexpected comment boundaries and code being accidentally commented out."
    },
    
    "5.8": {
        "title": "Identifiers that define objects or functions with external linkage shall be unique",
        "description": "External identifiers must be unique to avoid linking errors and undefined behavior when linking multiple translation units.",
        "rationale": "Non-unique external identifiers can cause linking errors and unpredictable behavior in multi-file projects."
    },
    
    "8.2": {
        "title": "Function types shall be in prototype form with named parameters",
        "description": "Function declarations should include parameter names in addition to types. This improves code readability and helps with documentation.",
        "rationale": "Named parameters in function prototypes make the interface clearer and improve code documentation."
    },
    
    "8.5": {
        "title": "An external object or function shall be declared once in one and only one file",
        "description": "External objects and functions should have exactly one declaration to avoid multiple definition errors and ensure consistent interfaces.",
        "rationale": "Multiple declarations can lead to inconsistencies and linking errors."
    },
    
    "8.6": {
        "title": "An identifier with external linkage shall have exactly one external definition",
        "description": "Each external identifier should be defined exactly once across the entire program to avoid multiple definition errors.",
        "rationale": "Multiple definitions of the same external identifier violate the one definition rule and can cause linking errors."
    },
    
    "8.7": {
        "title": "Functions and objects should not be defined with external linkage if they are referenced from only one translation unit",
        "description": "Functions and objects used only within a single file should be declared static to limit their scope and avoid namespace pollution.",
        "rationale": "Internal linkage prevents namespace pollution and allows for better optimization by the compiler."
    },
    
    "10.4": {
        "title": "Both operands of an operator in which the usual arithmetic conversions are performed shall have the same essential type",
        "description": "Arithmetic operations should be performed on operands of compatible types to avoid unexpected type conversions and potential loss of precision.",
        "rationale": "Mixed-type arithmetic can lead to unexpected results due to implicit type conversions."
    },
    
    "12.3": {
        "title": "The comma operator should not be used",
        "description": "The comma operator can make code difficult to understand and maintain. Its use should be avoided except in specific contexts like for-loop expressions.",
        "rationale": "The comma operator can obscure control flow and make code harder to read and debug."
    },
    
    "13.4": {
        "title": "The result of an assignment operator should not be used",
        "description": "Assignment expressions should not be used as sub-expressions. Assignments should be separate statements to improve readability and avoid confusion.",
        "rationale": "Using assignment results as expressions can make code harder to read and may hide logical errors."
    },
    
    "15.5": {
        "title": "A function should have a single point of exit at the end",
        "description": "Functions should have one return statement at the end rather than multiple return points throughout the function body. This improves code clarity and makes debugging easier.",
        "rationale": "Single exit points make functions easier to understand, debug, and maintain."
    },
    
    "16.3": {
        "title": "An unconditional break statement shall terminate every switch-clause",
        "description": "Every case in a switch statement should end with a break statement to prevent fall-through behavior, unless fall-through is explicitly intended and documented.",
        "rationale": "Missing break statements can cause unintended fall-through behavior that leads to bugs."
    },
    
    "17.3": {
        "title": "A function shall not be declared implicitly",
        "description": "All functions should be explicitly declared before use. Implicit function declarations can lead to type mismatches and undefined behavior.",
        "rationale": "Implicit function declarations can cause type mismatches and make code harder to understand and maintain."
    },
    
    "17.8": {
        "title": "A function parameter should not be modified",
        "description": "Function parameters should not be modified within the function body. If modification is needed, use a local copy of the parameter.",
        "rationale": "Modifying parameters can make function behavior less predictable and harder to understand."
    },
    
    "20.9": {
        "title": "All identifiers used in the controlling expression of #if or #elif preprocessing directives shall be #define'd before evaluation",
        "description": "Preprocessor identifiers used in conditional compilation should be explicitly defined to avoid undefined behavior in preprocessor evaluation.",
        "rationale": "Undefined preprocessor identifiers evaluate to 0, which may not be the intended behavior."
    },
    
    "21.2": {
        "title": "A reserved identifier or reserved macro name shall not be declared",
        "description": "Identifiers reserved by the C standard (such as those beginning with underscore) should not be used in user code to avoid conflicts with implementation-defined behavior.",
        "rationale": "Using reserved identifiers can conflict with compiler and standard library implementations."
    }
}

def get_rule_description(rule_id):
    """
    Get the description for a MISRA C 2012 rule.
    
    Args:
        rule_id (str): Rule identifier (e.g., "10.4", "misra-c2012-10.4")
    
    Returns:
        dict: Rule description with title, description, and rationale, or None if not found
    """
    # Normalize rule ID (remove misra-c2012- prefix if present)
    normalized_id = rule_id.replace("misra-c2012-", "").replace("misra-", "")
    
    return MISRA_RULE_DESCRIPTIONS.get(normalized_id)

def format_rule_description(rule_id, include_rationale=True):
    """
    Format a rule description for display.
    
    Args:
        rule_id (str): Rule identifier
        include_rationale (bool): Whether to include the rationale
    
    Returns:
        str: Formatted rule description
    """
    rule_info = get_rule_description(rule_id)
    if not rule_info:
        return "Rule description not available"
    
    result = f"**{rule_info['title']}**\n\n{rule_info['description']}"
    
    if include_rationale and 'rationale' in rule_info:
        result += f"\n\n*Rationale: {rule_info['rationale']}*"
    
    return result

def get_available_rules():
    """
    Get a list of all available rule IDs.
    
    Returns:
        list: List of available rule IDs
    """
    return list(MISRA_RULE_DESCRIPTIONS.keys())