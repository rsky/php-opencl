/**
 * The OpenCL PHP extension
 *
 * @package     php-opencl
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "kernel.h"

BEGIN_EXTERN_C()

PHPCL_LOCAL cl_int
php_cl_set_kernel_arg(cl_kernel kernel, cl_uint arg_index,
                      zval *arg_value, phpcl_c_type_t type TSRMLS_DC)
{
    return CL_SUCCESS;
}

END_EXTERN_C()

/*
* Local variables:
* tab-width: 4
* c-basic-offset: 4
* End:
* vim600: noet sw=4 ts=4 fdm=marker
* vim<600: noet sw=4 ts=4
*/
