# Security Policy

## Supported Versions

| Version | Supported          |
|----------|--------------------|
| 3.3.x   | ✅ Active support   |
| 3.2.x   | ✅ Security patches  |
| < 3.2   | ❌ No longer supported |

## Reporting a Vulnerability

This library may be used in safety-critical embedded systems. Security vulnerabilities should be reported promptly.

**To report a vulnerability:**

1. **Do NOT open a public issue.** Use [GitHub Security Advisories](https://github.com/aschokinatgmail/freertos_cpp_wrappers/security/advisories/new) for confidential reporting.
2. Provide a clear description including:
   - Affected version(s)
   - Steps to reproduce
   - Potential impact on safety-critical deployments
3. **Response timeline**: We aim to acknowledge within 48 hours and provide an initial assessment within 5 business days.

## Security Considerations for Embedded Systems

This library provides C++ wrappers for FreeRTOS APIs. Users should be aware:

- **Stack depth**: Task stacks are allocated by the caller; ensure adequate depth for wrapper overhead
- **ISR safety**: Functions marked `_isr` must only be called from ISR context
- **Memory allocation**: Use `static_allocator` variants for deterministic memory usage; dynamic allocators may exhaust heap
- **Error handling**: All operations return `freertos::expected<T,E>` — always check results
- **Thread safety**: Guards (`lock_guard`, `try_lock_guard`, `unique_lock`) provide RAII-based mutex protection
