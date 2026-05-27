# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**ft_snowcrash** is an educational cybersecurity project from École 42. This repository contains a structured learning path covering:
- Binary exploitation and reverse engineering fundamentals
- Memory management and security vulnerabilities
- Practical exercises with progressive difficulty levels
- Bonus challenges to deepen understanding

This is the **first project** in a cybersecurity series that builds foundational knowledge for subsequent advanced projects.

## Repository Structure

```
SnowCrash/
├── CLAUDE.md                 (this file - Claude guidance)
├── README.md                 (course overview and index)
├── SETUP.md                  (environment setup for Fedora)
├── 01-teoria/                (theoretical concepts and foundations)
│   ├── leccion_01.md        (Security concepts fundamentals)
│   ├── leccion_02.md        (Binary files and memory layout)
│   └── ...
├── 02-practicas/             (hands-on exercises)
│   ├── nivel_00/            (beginner exercises)
│   │   ├── ejercicio_01.md
│   │   ├── solucion_01.sh
│   │   └── notas.md
│   ├── nivel_01/            (intermediate exercises)
│   └── nivel_bonus/         (bonus challenges)
└── recursos/                 (tools, references, helpful materials)
    ├── herramientas.md      (required tools and installation)
    ├── referencias.md       (external resources and documentation)
    └── glosario.md          (cybersecurity terminology)
```

## Teaching Philosophy

This course follows a **strictly progressive pedagogical approach**:

1. **Explicit Examples**: Every concept includes concrete, detailed examples. Nothing is assumed to be "obvious."
2. **Theory First, Then Practice**: Each theory lesson is followed by hands-on exercises of increasing difficulty.
3. **Hints for Future Learning**: Where concepts relate to upcoming projects in the series, explicit hints are provided.
4. **Fedora-Focused**: All practical exercises are designed for Fedora (latest version) and include step-by-step terminal commands.

## Content Guidelines for Claude

When generating new content:

- **Explicitness Over Brevity**: Assume the reader is a complete beginner. Explain what seems obvious.
- **Code Examples**: All code examples should be:
  - Fully functional and tested on Fedora
  - Heavily commented explaining each step
  - Progressively more complex across exercises
- **Exercise Design**:
  - Start with simple, isolated concepts
  - Build complexity gradually
  - Include both "correct" solutions and common mistakes to learn from
- **Forward References**: Mark concepts that will be important in later projects (e.g., stack exploitation → next projects in series)
- **No Shortcuts**: Avoid "magic" commands; explain why each step exists

## Common Tasks

### Viewing the Course
- Start with `README.md` for the course index
- Follow the numbered lessons in `01-teoria/` sequentially
- Each theory lesson references corresponding exercises in `02-practicas/`

### Adding New Content
- Theory: Create `01-teoria/leccion_NN.md` following the template
- Exercises: Create subdirectory in `02-practicas/nivel_X/` with:
  - `ejercicio_NN.md` (instructions and learning objectives)
  - `solucion_NN.sh` (complete working solution)
  - `notas.md` (additional context and hints)
- All paths and commands must be tested on Fedora

### Verification
- All shell scripts must be executable: `chmod +x *.sh`
- All code examples must be tested on Fedora before inclusion
- Exercise solutions should include expected output examples

## Important Notes

- **Target Audience**: Beginners with little to no cybersecurity background
- **Environment**: Fedora Linux (latest version)
- **Delivery Format**: Markdown files + executable shell scripts
- **Scope**: Building foundational knowledge that prepares for advanced projects in the School 42 cybersecurity track
- **No Assumptions**: Do not assume familiarity with Linux, C, assembly, or security concepts

## Related Projects in the Series

This course prepares for:
- Next project: Advanced binary exploitation techniques
- Later projects: Kernel exploitation, malware analysis, etc.

(These will be referenced throughout with forward-looking hints.)
