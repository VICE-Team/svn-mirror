/*
 * mon_profiler.c -- Monitor Interface for CPU profiler
 *
 * Written by
 *  Oskar Linde <oskar.linde@gmail.com>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"

#include <stdio.h>
#include <string.h>

#include "lib.h"
#include "machine.h"
#include "maincpu.h"
#include "mon_profile.h"
#include "profiler.h"
#include "profiler_data.h"

const int min_label_width = 15;
static void print_disass_context(profiling_context_t *context, bool print_subcontexts);
static void print_function_line(profiling_context_t *context, int indent, profiling_counter_t total_cycles);

void mon_profile(void)
{
    if (maincpu_profiling) {
        mon_out("Profiling running.\n");
    } else if (!root_context) {
        mon_out("Profiling not started.\n");
    } else {
        mon_out("Profiling data available.\n");
    }
    mon_out("Use \"help prof\" for more information.\n");
}

void mon_profile_action(ACTION action)
{
    switch(action) {
    case e_OFF: {
        if (maincpu_profiling) {
            profile_stop();
            mon_out("Profiling stopped.\n");
        } else {
            mon_out("Profiling not started.\n");
        }
        return;
    }
    case e_ON: {
        profile_start();
        if (maincpu_profiling) {
            mon_out("Profiling restarted.\n");
        } else {
            mon_out("Profiling started.\n");
        }
        return;
    }
    case e_TOGGLE: {
        if (maincpu_profiling) {
            mon_profile_action(e_OFF);
        } else {
            mon_profile_action(e_ON);
        }
        return;
    }
    }
}

static bool init_profiling_data(void) {
    if (!root_context) {
        mon_out("No profiling data available. Start profiling with \"prof on\".\n");
        return false;
    }

    compute_aggregate_stats(root_context);

    return true;
}

static void aggregate_context(profiling_context_t *output,
                              profiling_context_t *source) {
    int i,j;

    output->num_enters        += source->num_enters;
    output->num_exits         += source->num_exits;
    output->total_cycles      += source->total_cycles;
    output->total_cycles_self += source->total_cycles_self;

    for (i = 0; i < 256; i++) {
        if (source->page[i]) {
            profiling_page_t *output_page = profiling_get_page(output, i);
            profiling_page_t *source_page = source->page[i];
            for (j = 0; j < 256; j++) {
                output_page->data[j].num_cycles  += source_page->data[j].num_cycles;
                output_page->data[j].num_samples += source_page->data[j].num_samples;
                output_page->data[j].touched     |= source_page->data[j].touched;
            }
        }
    }
}

static uint16_t parent_function(profiling_context_t *context) {
    if (context->parent) {
        return context->parent->pc_dst;
    } else {
        return 0x0000;
    }
}

/* acending based on pc_dst */
static int pc_dst_compare(void const* a, void const* b) {
    return (int)(*((profiling_context_t**)a))->pc_dst - (int)(*((profiling_context_t**)b))->pc_dst;
}

/* acending based on pc_dst */
static int sort_by_parent_function(void const* a, void const* b) {
    return (int)parent_function(*((profiling_context_t**)a)) - (int)parent_function(*((profiling_context_t**)b));
}

/* descending based on total_cycles_self */
static int self_time(void const* a, void const* b) {
    return (int)(*((profiling_context_t**)b))->total_cycles_self - (int)(*((profiling_context_t**)a))->total_cycles_self;
}

/* descending based on total_cycles */
static int total_time(void const* a, void const* b) {
    return (int)(*((profiling_context_t**)b))->total_cycles - (int)(*((profiling_context_t**)a))->total_cycles;
}

typedef struct context_array_s {
    profiling_context_t **data;
    int capacity;
    int size;
} context_array_t;

context_array_t all_functions;

static void array_sort(context_array_t *arr, int (*compar)(const void *, const void *))
{
    qsort(arr->data, arr->size, sizeof(*arr->data), compar);
}

static int binary_search(context_array_t *arr, profiling_context_t *ref, int (*compar)(const void *, const void *))
{
    int lo = 0;
    int hi = arr->size;
    while (hi > lo) {
        int mid = (lo+hi)/2;
        int res = compar(&arr->data[mid], &ref);
        if (res < 0) {
            lo = mid+1;
        } else if (res > 0) {
            hi = mid;
        } else {
            return mid;
        }
    }

    return arr->size;
}

static void array_append(context_array_t *arr, profiling_context_t *context) {
    arr->size++;

    if (arr->size > arr->capacity) {
        arr->capacity *= 2;
        if (arr->capacity < 10) {
            arr->capacity = 10;
        }
        arr->data = lib_realloc(arr->data, arr->capacity * sizeof(*arr->data));
    }

    arr->data[arr->size-1] = context;
}

static void array_free(context_array_t *arr) {
    lib_free(arr->data);
    arr->data = NULL;
    arr->size = 0;
    arr->capacity = 0;
}

/* assuming sorted source array */
static void array_compact(context_array_t * output, context_array_t const* source, int (*compar)(const void *, const void *)) {
    profiling_context_t * const* src_ptr = source->data;
    while (src_ptr != source->data + source->size) {
        profiling_context_t *aggregate = alloc_profiling_context();


        aggregate->pc_dst = (*src_ptr)->pc_dst;
        /* HACK, store parent->dst as source pointer */
        aggregate->pc_src = (*src_ptr)->parent->pc_dst;
        aggregate_context(aggregate, *src_ptr);

        while (src_ptr + 1 < source->data + source->size &&
               compar(src_ptr, src_ptr+1) == 0) {
            aggregate_context(aggregate, *(src_ptr+1));
            src_ptr++;
        }

        array_append(output, aggregate);

        src_ptr++;
    }
}


static void recursively_aggregate_all_functions(profiling_context_t *context) {
    /* binary search for matching context */
    int element = binary_search(&all_functions, context, pc_dst_compare);
    if (element < all_functions.size) {
        aggregate_context(all_functions.data[element], context);
    } else {
        /* no match, insert and sort */
        profiling_context_t *new_context = alloc_profiling_context();

        new_context->pc_dst = context->pc_dst;
        aggregate_context(new_context, context);
        array_append(&all_functions, new_context);
        array_sort(&all_functions, pc_dst_compare);
    }

    /* recursively add children */
    if (context->child) {
        profiling_context_t *c = context->child;
        do {
            recursively_aggregate_all_functions(c);
            c = c->next;
        } while (c != context->child);
    }
}


void mon_profile_flat(int num)
{
    int i;

    if (!init_profiling_data()) return;

    if (num <= 0) num = 20;

    all_functions.size = 0;
    all_functions.capacity = 0;
    all_functions.data = NULL;

    recursively_aggregate_all_functions(root_context);

    /* sort based on self time */
    array_sort(&all_functions, self_time);

    mon_out("        Total      %%          Self      %%\n");
    mon_out("------------- ------ ------------- ------\n");

    if (num > all_functions.size) num = all_functions.size;
    for (i = 0; i < num; i++) {
        print_function_line(all_functions.data[i], 0 /* indent */, root_context->total_cycles);
        free_profiling_context(all_functions.data[i]);
    }

    array_free(&all_functions);
}

static void print_context_graph(profiling_context_t *context, int depth, int max_depth, profiling_counter_t total_cycles);


void mon_profile_graph(int context_id, int depth)
{
    profiling_context_t * context;

    if (!init_profiling_data()) return;

    context = profile_context_by_id(context_id);
    if (!context) {
        context = root_context;
    }

    if (depth <= 0) depth = 4;

    mon_out("%*s        Total      %%          Self      %%\n", min_label_width + 15 + depth*2+1, "");
    mon_out("%*s------------- ------ ------------- ------\n", min_label_width + 15 + depth*2+1, "");

    print_context_graph(context, 0, depth, context->total_cycles);
}

static void recursively_aggregate_function_profile(profiling_context_t *context,
                                            uint16_t             addr,
                                            context_array_t     *callers,
                                            context_array_t     *callees,
                                            profiling_context_t *aggregate)
{
    if (context->pc_dst == addr) {
        aggregate_context(aggregate, context);
        array_append(callers, context);
    }

    if (context->child) {
        profiling_context_t *c = context->child;
        do {
            if (context->pc_dst == addr) {
                array_append(callees, c);
            }
            recursively_aggregate_function_profile(c, addr, callers, callees, aggregate);
            c = c->next;
        } while (c != context->child);
    }
}

void mon_profile_func(MON_ADDR function)
{
    context_array_t callers        = {NULL, 0, 0};
    context_array_t callers_merged = {NULL, 0, 0};
    context_array_t callees        = {NULL, 0, 0};
    context_array_t callees_merged = {NULL, 0, 0};
    profiling_context_t *aggregate = NULL;
    uint16_t addr;
    int i;

    if (!init_profiling_data()) return;

    if (addr_memspace(function) == e_default_space) {
        addr = addr_location(function);
    } else {
        mon_out("Invalid address space\n");
        return;
    }

    aggregate = alloc_profiling_context();
    aggregate->pc_dst = addr;

    recursively_aggregate_function_profile(root_context, addr, &callers, &callees, aggregate);

    /* merge all callers that have the same parent function */
    array_sort(&callers, sort_by_parent_function);
    array_compact(&callers_merged, &callers, sort_by_parent_function);
    array_sort(&callers_merged, self_time);

    /* merge all calleess that have the same target function */
    array_sort(&callees, pc_dst_compare);
    array_compact(&callees_merged, &callees, pc_dst_compare);
    array_sort(&callees_merged, total_time);
    mon_out("                                          Callers\n");
    mon_out("        Total      %%          Self      %%         Callees\n");
    mon_out("------------- ------ ------------- ------ |---|---|------\n");

    for (i = 0; i < callers_merged.size; i++) {
        /* HACK: the function name is stored in pc_src */
        callers_merged.data[i]->pc_dst = callers_merged.data[i]->pc_src;
        print_function_line(callers_merged.data[i], 0 /* indent */, aggregate->total_cycles);
        free_profiling_context(callers_merged.data[i]);
    }

    print_function_line(aggregate, 4 /* indent */, aggregate->total_cycles);

    for (i = 0; i < callees_merged.size; i++) {
        print_function_line(callees_merged.data[i], 8 /* indent */, aggregate->total_cycles);
        free_profiling_context(callees_merged.data[i]);
    }

    free_profiling_context(aggregate);
    array_free(&callers);
    array_free(&callers_merged);
    array_free(&callees);
    array_free(&callees_merged);
}

static bool is_interrupt(uint16_t src) {
    return src >= 0xfffa && !(src & 1);
}

static void print_src(uint16_t src) {
    switch(src) {
    case 0x0000: mon_out("BOOT"); return;
    case 0xfffa: mon_out("NMI "); return;
    case 0xfffc: mon_out("RST "); return;
    case 0xfffe: mon_out("IRQ "); return;
    /* subtract 2 to show the start of the JSR instruction */
    default: mon_out("%04x", (unsigned)(src-2)); return;
    }
}

static void print_dst(uint16_t dst, int max_width) {
    char *name = mon_symbol_table_lookup_name(default_memspace, dst);
    if (name) {
        size_t l = strlen(name);
        if (l > max_width) {
            mon_out("%*.*s...", -(max_width-3), (max_width-3), name);
        } else {
            mon_out("%*s", -max_width, name);
        }
    } else if (dst == 0x0000) {
        mon_out("ROOT");
        mon_out("%*s", max_width-4, "");
    } else {
        mon_out("%04x", dst);
        mon_out("%*s", max_width-4, "");
    }
}

static void print_context_id(profiling_context_t *context, int max_width) {
    char idstr[16];
    snprintf(idstr, 16, "[%d] ", get_context_id(context));
    mon_out("%*s", max_width, idstr);
}

static void print_context_name(profiling_context_t *context, int indent, int max_indent) {
    print_context_id(context, indent + 7);
    if (context->pc_dst == 0) {
        mon_out("START   ");
        mon_out("%*s", min_label_width + max_indent - indent, "");
    } else {
        print_src(context->pc_src);
        mon_out(" -> ");
        print_dst(context->pc_dst, min_label_width + max_indent - indent);
    }
}

static void print_function_line(profiling_context_t *context, int indent, profiling_counter_t total_cycles) {
    mon_out("%'13u %5.1f%% ", context->total_cycles,      100.0 * context->total_cycles / total_cycles);
    mon_out("%'13u %5.1f%% ", context->total_cycles_self, 100.0 * context->total_cycles_self / total_cycles);
    mon_out("%*s", indent, "");
    print_dst(context->pc_dst, 40);
    mon_out("\n");
}

static void print_context_line(profiling_context_t *context, int indent, int max_indent, profiling_counter_t total_cycles) {
    print_context_name(context, indent, max_indent);

    mon_out("%'13u %5.1f%% ",  context->total_cycles,      100.0 * context->total_cycles / total_cycles);
    mon_out("%'13u %5.1f%%\n", context->total_cycles_self, 100.0 * context->total_cycles_self / total_cycles);
}

static void print_context_graph(profiling_context_t *context, int depth, int max_depth, profiling_counter_t total_cycles)
{
    if (context == current_context) {
        mon_out(">");
    } else {
        mon_out(" ");
    }

    print_context_line(context, depth * 2, max_depth * 2, total_cycles);

    if (context->child) {
        if (depth < max_depth)  {
            profiling_context_t *c = context->child;
            do {
                print_context_graph(c, depth + 1, max_depth, total_cycles);
                c = c->next;
            } while (c != context->child);
        } else {
            /* check if we are a child to the current_context */
            profiling_context_t *c = current_context;
            do {
                c = c->parent;
                if (c == context) {
                    /* if so, print cursor */
                    mon_out(">");
                    break;
                }
            } while(c);

            if (!c) {
                mon_out(" ");
            }

            mon_out("%*s...\n", (depth+1)*2 + 8, "");
        }
    }
}

static void aggregate_context_recursively(profiling_context_t *output,
                                   profiling_context_t *source) {

    if (output->pc_dst == source->pc_dst) {
        aggregate_context(output, source);
    }

    if (source->child) {
        profiling_context_t *c = source->child;
        do {
            if (output->pc_dst == source->pc_dst) {
                /* copy children */
                profiling_context_t *child_copy = alloc_profiling_context();
                child_copy->pc_src = c->pc_src;
                child_copy->pc_dst = c->pc_dst;
                child_copy->total_cycles = c->total_cycles;
                child_copy->total_cycles_self = c->total_cycles_self;
                if (!output->child) {
                    output->child = child_copy;
                    output->child->next = output->child;
                } else {
                    child_copy->next = output->child->next;
                    output->child->next = child_copy;
                }
            }
            aggregate_context_recursively(output, c);
            c = c->next;
        } while (c != source->child);
    }
}

static void print_all_contexts(profiling_context_t * context, uint16_t dst_addr)
{
    if (context->pc_dst == dst_addr) {
        mon_out("[%d]", get_context_id(context));
    }

    if (context->child) {
        profiling_context_t *c = context->child;
        do {
            print_all_contexts(c, dst_addr);
            c = c->next;
        } while(c != context->child);
    }
}

void mon_profile_disass(MON_ADDR function)
{
    uint16_t addr;
    profiling_context_t *context;
    if (!init_profiling_data()) return;

    if (addr_memspace(function) == e_default_space) {
        addr = addr_location(function);
    } else {
        mon_out("Invalid address space\n");
        return;
    }

    context = alloc_profiling_context();
    context->pc_dst = addr;

    aggregate_context_recursively(context, root_context);

    mon_out("\n");

    mon_out("Function ");
    print_dst(addr, 70);
    mon_out("\n");

    mon_out("\n");

    mon_out("   Contexts ");
    print_all_contexts(root_context, addr);
    mon_out("\n");

    mon_out("\n");

    print_disass_context(context, false /* print_subcontexts */);

    /* this also frees children */
    free_profiling_context(context);
}

static void print_cycle_time(double cycles, int align_column) {
    double time = cycles / machine_get_cycles_per_second();
    char  *unit;
    char  *format = "%*.*f %s";
    int    precision;

    /* try to maintain 3 significant figures */

    if (time >= 1.0) {
        unit = "s";
    } else if (time >= 1e-3) {
        unit = "ms";
        time *= 1e3;
    } else {
        unit = "us";
        time *= 1e6;
    }

    if      (time >= 100)  precision = 0;
    else if (time >= 10)   precision = 1;
    else                   precision = 2;

    mon_out(format, align_column, precision, time, unit);
}

static void print_disass_context(profiling_context_t *context, bool print_contexts) {
    profiling_context_t *c;
    int num_memory_map_configs = 0;
    int i, j;
    uint16_t next_addr = 0;
    double average_times;
    int context_column = 0;
    int addr_column = 0;

    average_times = 0.5 * (context->num_enters + context->num_exits);

    mon_out("   Entered %'10u time%s\n", context->num_enters, context->num_enters != 1 ? "s":"");
    mon_out("   Exited  %'10u time%s\n", context->num_exits,  context->num_exits!= 1 ? "s":"");

    mon_out("   Total   %'10u cycles ", context->total_cycles);
    print_cycle_time(context->total_cycles, 10);
    mon_out("\n");

    mon_out("   Self    %'10u cycles ", context->total_cycles_self);
    print_cycle_time(context->total_cycles_self, 10);
    mon_out("\n");

    mon_out("   Average %'10.0f cycles ", context->total_cycles/average_times);
    print_cycle_time(context->total_cycles/average_times, 10);
    if (context->num_enters != context->num_exits) {
        mon_out("*\n");
        mon_out("        * Averages are not accurate since enters differs from exits.");
    }
    mon_out("\n\n");
    mon_out("   Memory Banking Config%s:", context->next_mem_config ? "s" : "");
    c = context;
    while (c) {
        num_memory_map_configs++;
        mon_out(" %d", c->memory_bank_config);
        c = c->next_mem_config;
    }
    mon_out("\n\n");

    addr_column = 37;
    if (print_contexts) {
        context_column = 7;
        addr_column   += 7;
    }
    if (num_memory_map_configs > 1) {
        addr_column += 5;
    }

    mon_out("       Cycles      %%         Times");
    if (print_contexts) mon_out(" Context");
    if (num_memory_map_configs > 1) mon_out(" Bank");
    mon_out(" Address  Disassembly\n");

    mon_out("------------- ------ -------------");
    if (print_contexts) mon_out(" -------");
    if (num_memory_map_configs > 1) mon_out(" ----");
    mon_out(" -------  -------------------------\n");

    for (i = 0; i < 256; i++) {
        bool any_access_to_page = false;
        c = context;
        while (c) { /* loop over different memory bank configs for each page */
            if (c->page[i]) {
                any_access_to_page = true;
                break;
            }
            c = c->next_mem_config;
        }

        if (any_access_to_page) {
            for (j = 0; j < 256; j++) {
                uint16_t addr = i << 8 | j;
                bool printed_instruction_at_addr = false;

                c = context;
                while (c) { /* loop over different memory bank configs for each page */
                    profiling_page_t *page = c->page[i];
                    if (page && (page->data[j].num_cycles > 0 || page->data[j].touched)) {
                        profiling_counter_t total_cycles = page->data[j].num_cycles;
                        unsigned opc_size;
                        profiling_context_t *subcontext = NULL;

                        if (addr != next_addr && next_addr != 0) {
                            mon_out("%*s...\n", addr_column, "");
                        }

                        /* note: using main 'context', not 'c' here */
                        if (context->child) {
                            profiling_context_t *child = context->child;
                            do {
                                /* -2 to get to the start of the JSR instruction */
                                if (child->pc_src - 2 == addr &&
                                    child->memory_bank_config == c->memory_bank_config) {
                                    total_cycles += child->total_cycles;
                                    subcontext = child;
                                }
                                child = child->next;
                            } while (child != context->child);
                        }

                        mon_out("%'13u %5.1f%% %'13u",
                                total_cycles,
                                100.0 * total_cycles / context->total_cycles,
                                page->data[j].num_samples);

                        if (print_contexts && subcontext) {
                            print_context_id(subcontext, context_column);
                        } else {
                            mon_out("%*s", context_column, "");
                        }

                        if (num_memory_map_configs > 1) {
                            mon_out("%5d", c->memory_bank_config);
                        }

                        if (addr == reg_pc) {
                            mon_out(">");
                        } else if (addr == context->pc_dst &&
                                   c    == context) {
                            mon_out("*");
                        } else if (printed_instruction_at_addr) {
                            mon_out("/");
                        } else {
                            mon_out(" ");
                        }

                        printed_instruction_at_addr = true;

                        if (is_interrupt(addr)) {
                            print_src(addr);
                            mon_out("\n");
                        } else {
                            opc_size = mon_disassemble_oneline(e_comp_space, c->memory_bank_config, addr);
                            next_addr = addr + opc_size;
                        }
                    }
                    c = c->next_mem_config;
                }
            }
        }
    }
}

void mon_profile_disass_context(int context_id)
{
    profiling_context_t *context;


    if (!init_profiling_data()) return;

    context = profile_context_by_id(context_id);

    if (!context) {
        mon_out("Invalid context. Use \"prof graph\" to list contexts.\n");
        return;
    }

    mon_out("\n");
    mon_out("Context ");
    print_context_name(context, 0, 20);
    mon_out("\n");

    if (context->parent) {
        mon_out("\n");
        mon_out("   Parent  ");
        print_context_name(context->parent, 0, 20);
        mon_out("\n");
    }
    mon_out("\n");

    print_disass_context(context, true /* print subcontexts */);
}

static void clear_context_pages(profiling_context_t *context) {
    int i,j;
    for (i = 0; i < 256; i++) {
        profiling_page_t *page = context->page[i];
        if (page) {
            for (j = 0; j < 256; j++) {
                if (page->data[j].num_samples > 0) {
                    page->data[j].num_cycles = 0;
                    page->data[j].touched = 1;
                }
            }
        }
    }

    if (context->next_mem_config) {
        clear_context_pages(context->next_mem_config);
    }
}

static void clear_recursively(profiling_context_t *context, uint16_t addr) {
    if (context->pc_dst == addr) {
        clear_context_pages(context);
    }

    if (context->child) {
        profiling_context_t *c = context->child;
        do {
            clear_recursively(c, addr);
            c = c->next;
        } while (c != context->child);
    }
}

void mon_profile_clear(MON_ADDR function)
{
    uint16_t addr;
    if (!init_profiling_data()) return;

    if (addr_memspace(function) == e_default_space) {
        addr = addr_location(function);
    } else {
        mon_out("Invalid address space\n");
        return;
    }

    clear_recursively(root_context, addr);
}

