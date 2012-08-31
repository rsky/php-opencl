/**
 * The OpenCL PHP extension
 *
 * @package     php-opencl
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#ifndef PHP_OPENCL_H
#define PHP_OPENCL_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
 
#include <php.h>
#include <php_ini.h>
#include <SAPI.h>
#include <ext/standard/info.h>
#include <Zend/zend_extensions.h>
#include <OpenCL/opencl.h>

#define PHPCL_VERSION "0.0.1"

/* {{{ type definitions */

typedef struct {
	cl_context context;
} phpcl_context_t;

typedef struct {
	cl_command_queue command_queue;
} phpcl_command_queue_t;

typedef struct {
	cl_mem mem;
} phpcl_mem_t;

typedef struct {
	cl_program program;
} phpcl_program_t;

typedef struct {
	cl_kernel kernel;
} phpcl_kernel_t;

typedef struct {
	cl_event event;
} phpcl_event_t;

typedef struct {
	cl_sampler sampler;
} phpcl_sampler_t;

/* }}} */
/* {{{ common functions */

const char *phpcl_errstr(cl_int err);

int phpcl_le_platform(void);
int phpcl_le_device(void);
int phpcl_le_context(void);
int phpcl_le_command_queue(void);
int phpcl_le_mem(void);
int phpcl_le_event(void);
int phpcl_le_program(void);
int phpcl_le_kernel(void);
int phpcl_le_sampler(void);

/* }}} */

#endif /* PHP_OPENCL_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
