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

typedef enum {
	/* scalar */
	C_TYPE_BOOL,
	C_TYPE_CHAR,
	C_TYPE_UCHAR,
	C_TYPE_SHORT,
	C_TYPE_USHORT,
	C_TYPE_INT,
	C_TYPE_UINT,
	C_TYPE_LONG,
	C_TYPE_ULONG,
	C_TYPE_FLOAT,
	C_TYPE_HALF,
	C_TYPE_DOUBLE,
	C_TYPE_SIZE,
	C_TYPE_PTRDIFF,
	C_TYPE_INTPTR,
	C_TYPE_UINTPTR,
	C_TYPE_VOID,
	/* vector */
	C_TYPE_CHAR2,
	C_TYPE_CHAR4,
	C_TYPE_CHAR8,
	C_TYPE_CHAR16,
	C_TYPE_UCHAR2,
	C_TYPE_UCHAR4,
	C_TYPE_UCHAR8,
	C_TYPE_UCHAR16,
	C_TYPE_SHORT2,
	C_TYPE_SHORT4,
	C_TYPE_SHORT8,
	C_TYPE_SHORT16,
	C_TYPE_USHORT2,
	C_TYPE_USHORT4,
	C_TYPE_USHORT8,
	C_TYPE_USHORT16,
	C_TYPE_INT2,
	C_TYPE_INT4,
	C_TYPE_INT8,
	C_TYPE_INT16,
	C_TYPE_UINT2,
	C_TYPE_UINT4,
	C_TYPE_UINT8,
	C_TYPE_UINT16,
	C_TYPE_LONG2,
	C_TYPE_LONG4,
	C_TYPE_LONG8,
	C_TYPE_LONG16,
	C_TYPE_ULONG2,
	C_TYPE_ULONG4,
	C_TYPE_ULONG8,
	C_TYPE_ULONG16,
	C_TYPE_FLOAT2,
	C_TYPE_FLOAT4,
	C_TYPE_FLOAT8,
	C_TYPE_FLOAT16,
	C_TYPE_UFLOAT2,
	C_TYPE_UFLOAT4,
	C_TYPE_UFLOAT8,
	C_TYPE_UFLOAT16,
	C_TYPE_HALF2,
	C_TYPE_HALF4,
	C_TYPE_HALF8,
	C_TYPE_HALF16,
	C_TYPE_DOUBLE2,
	C_TYPE_DOUBLE4,
	C_TYPE_DOUBLE8,
	C_TYPE_DOUBLE16,
	/* other */
	C_TYPE_IMAGE2D,
	C_TYPE_IMAGE3D,
	C_TYPE_SAMPLER,
	C_TYPE_EVENT,
	/* pointer */
	C_TYPE_MEM_PTR,
	C_TYPE_NULL_PTR
} phpcl_opencl_c_type_t;

typedef struct {
	cl_context context;
	zval *callback;
	zval *data;
} phpcl_context_t;

typedef struct {
	cl_mem memobj;
	void *ptr;
} phpcl_memobj_t;

typedef struct {
	cl_program program;
	zval *callback;
	zval *data;
} phpcl_program_t;

typedef enum {
	INFO_TYPE_BITFIELD = 0,
	INFO_TYPE_BOOL,
	INFO_TYPE_SIZE,
	INFO_TYPE_UINT,
	INFO_TYPE_ULONG,
	INFO_TYPE_STRING,
	INFO_TYPE_PLATFORM,
	INFO_TYPE_EXTRA,
} phpcl_info_type_t;

typedef struct {
	const char *key;
	cl_uint name;
	phpcl_info_type_t type;
} phpcl_info_param_t;

typedef cl_int (*phpcl_get_info_func_t)(void *,     /* obj1 */
                                        void *,     /* obj2 */
                                        cl_uint,    /* name */
                                        size_t,     /* value_size */
                                        void *,     /* value */
                                        size_t *    /* value_size_ret */);

typedef zval * (*phpcl_get_info_ex_func_t)(void *,  /* obj1 */
                                           void *,  /* obj2 */
                                           cl_uint  /* name */
                                           TSRMLS_DC);

/* }}} */
/* {{{ common functions */

const char *phpcl_errstr(cl_int errcode);

zval *phpcl_get_info(phpcl_get_info_func_t get_info,
                     phpcl_get_info_ex_func_t get_info_ex,
                     void *obj1, void *obj2,
                     const phpcl_info_param_t *param TSRMLS_DC);

cl_device_id *phpcl_context_get_devices(cl_context context,
                                        cl_uint *num_devices_ret,
                                        cl_int *errcode_ret);

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
