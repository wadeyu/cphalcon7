
/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2014 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  |          ZhuZongXin <dreamsxin@qq.com>                                 |
  +------------------------------------------------------------------------+
*/

#include "binary/writer.h"
#include "binary.h"
#include "binary/exception.h"

#include <ext/standard/php_array.h>
#include <ext/spl/spl_array.h>

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/fcall.h"
#include "kernel/string.h"
#include "kernel/array.h"
#include "kernel/operators.h"
#include "kernel/concat.h"
#include "kernel/object.h"
#include "kernel/exception.h"

#include "interned-strings.h"

/**
 * Phalcon\Binary\Writer
 *
 * Provides utilities to work with binary data
 */
zend_class_entry *phalcon_binary_writer_ce;

PHP_METHOD(Phalcon_Binary_Writer, __construct);
PHP_METHOD(Phalcon_Binary_Writer, getEndian);
PHP_METHOD(Phalcon_Binary_Writer, getOutput);
PHP_METHOD(Phalcon_Binary_Writer, getContent);
PHP_METHOD(Phalcon_Binary_Writer, getPosition);
PHP_METHOD(Phalcon_Binary_Writer, write);
PHP_METHOD(Phalcon_Binary_Writer, writeChar);
PHP_METHOD(Phalcon_Binary_Writer, writeUnsignedChar);
PHP_METHOD(Phalcon_Binary_Writer, writeInt16);
PHP_METHOD(Phalcon_Binary_Writer, writeUnsignedInt16);
PHP_METHOD(Phalcon_Binary_Writer, writeInt);
PHP_METHOD(Phalcon_Binary_Writer, writeUnsignedInt);
PHP_METHOD(Phalcon_Binary_Writer, writeInt32);
PHP_METHOD(Phalcon_Binary_Writer, writeUnsignedInt32);
PHP_METHOD(Phalcon_Binary_Writer, writeFloat);
PHP_METHOD(Phalcon_Binary_Writer, writeDouble);
PHP_METHOD(Phalcon_Binary_Writer, writeString);
PHP_METHOD(Phalcon_Binary_Writer, writeHexString);

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_binary___construct, 0, 0, 0)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, endian)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_binary_write, 0, 0, 2)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_binary_writechar, 0, 0, 1)
	ZEND_ARG_INFO(0, byte)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_binary_writeunsignedchar, 0, 0, 1)
	ZEND_ARG_INFO(0, byte)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_binary_writeint16, 0, 0, 1)
	ZEND_ARG_INFO(0, num)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_binary_writeunsignedint16, 0, 0, 1)
	ZEND_ARG_INFO(0, num)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_binary_writeint, 0, 0, 1)
	ZEND_ARG_INFO(0, num)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_binary_writeunsignedint, 0, 0, 1)
	ZEND_ARG_INFO(0, num)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_binary_writeint32, 0, 0, 1)
	ZEND_ARG_INFO(0, num)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_binary_writeunsignedint32, 0, 0, 1)
	ZEND_ARG_INFO(0, num)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_binary_writefloat, 0, 0, 1)
	ZEND_ARG_INFO(0, num)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_binary_writedouble, 0, 0, 1)
	ZEND_ARG_INFO(0, num)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_binary_writestring, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_INFO(0, exact)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phalcon_binary_writehexstring, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 1)
	ZEND_ARG_INFO(0, lowNibble)
ZEND_END_ARG_INFO()

static const zend_function_entry phalcon_arr_method_entry[] = {
	PHP_ME(Phalcon_Binary_Writer, __construct, arginfo_phalcon_binary___construct, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Phalcon_Binary_Writer, getEndian, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Binary_Writer, getOutput, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Binary_Writer, getContent, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Binary_Writer, getPosition, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Binary_Writer, write, arginfo_phalcon_binary_write, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Binary_Writer, writeChar, arginfo_phalcon_binary_writechar, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Binary_Writer, writeUnsignedChar, arginfo_phalcon_binary_writeunsignedchar, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Binary_Writer, writeInt16, arginfo_phalcon_binary_writeint16, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Binary_Writer, writeUnsignedInt16, arginfo_phalcon_binary_writeunsignedint16, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Binary_Writer, writeInt, arginfo_phalcon_binary_writeint, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Binary_Writer, writeUnsignedInt, arginfo_phalcon_binary_writeunsignedint, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Binary_Writer, writeInt32, arginfo_phalcon_binary_writeint32, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Binary_Writer, writeUnsignedInt32, arginfo_phalcon_binary_writeunsignedint32, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Binary_Writer, writeFloat, arginfo_phalcon_binary_writefloat, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Binary_Writer, writeDouble, arginfo_phalcon_binary_writedouble, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Binary_Writer, writeString, arginfo_phalcon_binary_writestring, ZEND_ACC_PUBLIC)
	PHP_ME(Phalcon_Binary_Writer, writeHexString, arginfo_phalcon_binary_writehexstring, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

/**
 * Phalcon\Binary initializer
 */
PHALCON_INIT_CLASS(Phalcon_Binary_Writer){

	PHALCON_REGISTER_CLASS(Phalcon\\Binary, Writer, binary_writer, phalcon_arr_method_entry, 0);

	zend_declare_property_long(phalcon_binary_writer_ce, SL("_endian"), PHALCON_BINARY_ENDIAN_BIG, ZEND_ACC_PROTECTED);
	zend_declare_property_null(phalcon_binary_writer_ce, SL("_output"), ZEND_ACC_PROTECTED);
	zend_declare_property_long(phalcon_binary_writer_ce, SL("_position"), 0, ZEND_ACC_PROTECTED);
	return SUCCESS;
}

/**
 * Phalcon\Binary\Writer constructor
 *
 * @param  string|resource $data
 * @param  int $endian
 * @throws \InvalidArgumentException
 */
PHP_METHOD(Phalcon_Binary_Writer, __construct){

	zval *data = NULL, *endian = NULL, filename = {}, mode = {}, handler = {}, fstat = {}, size = {};

	phalcon_fetch_params(0, 0, 2, &data, &endian);

	if (!data) {
		data = &PHALCON_GLOBAL(z_null);
	}

	if (Z_TYPE_P(data) == IS_STRING || Z_TYPE_P(data) == IS_NULL) {
		ZVAL_STRING(&filename, "php://memory");
		ZVAL_STRING(&mode, "br+");
		PHALCON_CALL_FUNCTIONW(&handler, "fopen", &filename, &mode);
		PHALCON_CALL_FUNCTIONW(NULL, "fwrite", &handler, data);

		PHALCON_CALL_FUNCTIONW(&fstat, "fstat", &handler);
		if (phalcon_array_isset_fetch_str(&size, &fstat, SL("size"))) {
			phalcon_update_property_zval(getThis(), SL("_position"), &size);
		}
		phalcon_update_property_zval(getThis(), SL("_output"), &handler);
	} else if (Z_TYPE_P(data) == IS_RESOURCE) {
		phalcon_update_property_zval(getThis(), SL("_output"), data);

		PHALCON_CALL_FUNCTIONW(&fstat, "fstat", data);
		if (phalcon_array_isset_fetch_str(&size, &fstat, SL("size"))) {
			phalcon_update_property_zval(getThis(), SL("_position"), &size);
		}
	} else {
		PHALCON_THROW_EXCEPTION_STRW(phalcon_binary_exception_ce, "Data must be set as string or resource");
		return;
	}

	if (endian && Z_TYPE_P(endian) != IS_NULL) {
		if (Z_TYPE_P(endian) != IS_LONG || Z_LVAL_P(endian) < 0 || Z_LVAL_P(endian) > 2) {
			PHALCON_THROW_EXCEPTION_STRW(phalcon_binary_exception_ce, "Endian must be set as big or little");
		}
		phalcon_update_property_zval(getThis(), SL("_endian"), endian);
	}
}

/**
 * Gets the endian
 *
 * @return int
 */
PHP_METHOD(Phalcon_Binary_Writer, getEndian){


	RETURN_MEMBER(getThis(), "_endian");
}

/**
 * Gets the ouput
 *
 * @return int
 */
PHP_METHOD(Phalcon_Binary_Writer, getOutput){


	RETURN_MEMBER(getThis(), "_ouput");
}

/**
 * Gets the ouput
 *
 * @return int
 */
PHP_METHOD(Phalcon_Binary_Writer, getContent){

	zval output = {}, position = {};

	phalcon_read_property(&output, getThis(), SL("_output"), PH_NOISY);
	phalcon_read_property(&position, getThis(), SL("_position"), PH_NOISY);

	PHALCON_CALL_FUNCTIONW(NULL, "rewind", &output);
	PHALCON_CALL_FUNCTIONW(return_value, "fread", &output, &position);
}

/**
 * Gets the current postion
 *
 * @return int
 */
PHP_METHOD(Phalcon_Binary_Writer, getPosition){


	RETURN_MEMBER(getThis(), "_position");
}

/**
 *
 */
PHP_METHOD(Phalcon_Binary_Writer, write){

	zval *data, *length, position = {}, result = {}, output = {};

	phalcon_fetch_params(0, 2, 0, &data, &length);

	phalcon_read_property(&position, getThis(), SL("_position"), PH_NOISY);
	phalcon_add_function(&result, &position, length);

	phalcon_read_property(&output, getThis(), SL("_output"), PH_NOISY);
	PHALCON_CALL_FUNCTIONW(return_value, "fwrite", &output, data, length);
	phalcon_update_property_zval(getThis(), SL("_position"), &result);
}

/**
 *
 */
PHP_METHOD(Phalcon_Binary_Writer, writeChar){

	zval *byte, length = {}, format = {}, result = {};

	phalcon_fetch_params(0, 1, 0, &byte);

	ZVAL_LONG(&length, 1);

	ZVAL_STRING(&format, "c");
	PHALCON_CALL_FUNCTIONW(&result, "pack", &format, byte);

	PHALCON_CALL_METHODW(NULL, getThis(), "write", &result, &length);
	RETURN_THISW();
}

/**
 *
 */
PHP_METHOD(Phalcon_Binary_Writer, writeUnsignedChar){

	zval *byte, length = {}, format = {}, result = {};

	phalcon_fetch_params(0, 1, 0, &byte);

	ZVAL_LONG(&length, 1);

	ZVAL_STRING(&format, "C");
	PHALCON_CALL_FUNCTIONW(&result, "pack", &format, byte);

	PHALCON_CALL_METHODW(NULL, getThis(), "write", &result, &length);
	RETURN_THISW();
}

/**
 *
 */
PHP_METHOD(Phalcon_Binary_Writer, writeInt16){

	zval *num, length = {}, format = {}, result = {};

	phalcon_fetch_params(0, 1, 0, &num);

	ZVAL_LONG(&length, 2);

	ZVAL_STRING(&format, "s");
	PHALCON_CALL_FUNCTIONW(&result, "pack", &format, num);

	PHALCON_CALL_METHODW(NULL, getThis(), "write", &result, &length);
	RETURN_THISW();
}

/**
 *
 */
PHP_METHOD(Phalcon_Binary_Writer, writeUnsignedInt16){

	zval *num, length = {}, endian = {}, format = {}, result = {};

	phalcon_fetch_params(0, 1, 0, &num);

	ZVAL_LONG(&length, 2);

	phalcon_read_property(&endian, getThis(), SL("_endian"), PH_NOISY);
	if (Z_LVAL(endian) == PHALCON_BINARY_ENDIAN_BIG) {
		ZVAL_STRING(&format, "n");
	} else if (Z_LVAL(endian) == PHALCON_BINARY_ENDIAN_LITTLE) {
		ZVAL_STRING(&format, "v");
	} else if (Z_LVAL(endian) == PHALCON_BINARY_ENDIAN_MACHINE) {
		ZVAL_STRING(&format, "S");
	}

	PHALCON_CALL_FUNCTIONW(&result, "pack", &format, num);
	PHALCON_CALL_METHODW(NULL, getThis(), "write", &result, &length);
	RETURN_THISW();
}

/**
 *
 */
PHP_METHOD(Phalcon_Binary_Writer, writeInt){

	zval *num, length = {}, format = {}, result = {};

	phalcon_fetch_params(0, 1, 0, &num);

	ZVAL_LONG(&length, sizeof(int));

	ZVAL_STRING(&format, "i");
	PHALCON_CALL_FUNCTIONW(&result, "pack", &format, num);

	PHALCON_CALL_METHODW(NULL, getThis(), "write", &result, &length);
	RETURN_THISW();
}

/**
 *
 */
PHP_METHOD(Phalcon_Binary_Writer, writeUnsignedInt){

	zval *num, length = {}, format = {}, result = {};

	phalcon_fetch_params(0, 1, 0, &num);

	ZVAL_LONG(&length, sizeof(int));

	ZVAL_STRING(&format, "I");
	PHALCON_CALL_FUNCTIONW(&result, "pack", &format, num);

	PHALCON_CALL_METHODW(NULL, getThis(), "write", &result, &length);
	RETURN_THISW();
}

/**
 *
 */
PHP_METHOD(Phalcon_Binary_Writer, writeInt32){

	zval *num, length = {}, format = {}, result = {};

	phalcon_fetch_params(0, 1, 0, &num);

	ZVAL_LONG(&length, 4);

	ZVAL_STRING(&format, "l");
	PHALCON_CALL_FUNCTIONW(&result, "pack", &format, num);

	PHALCON_CALL_METHODW(NULL, getThis(), "write", &result, &length);
	RETURN_THISW();
}

/**
 *
 */
PHP_METHOD(Phalcon_Binary_Writer, writeUnsignedInt32){

	zval *num, length = {}, endian = {}, format = {}, result = {};

	phalcon_fetch_params(0, 1, 0, &num);

	ZVAL_LONG(&length, 4);

	phalcon_read_property(&endian, getThis(), SL("_endian"), PH_NOISY);
	if (Z_LVAL(endian) == PHALCON_BINARY_ENDIAN_BIG) {
		ZVAL_STRING(&format, "N");
	} else if (Z_LVAL(endian) == PHALCON_BINARY_ENDIAN_LITTLE) {
		ZVAL_STRING(&format, "V");
	} else if (Z_LVAL(endian) == PHALCON_BINARY_ENDIAN_MACHINE) {
		ZVAL_STRING(&format, "L");
	}

	PHALCON_CALL_FUNCTIONW(&result, "pack", &format, num);
	PHALCON_CALL_METHODW(NULL, getThis(), "write", &result, &length);
	RETURN_THISW();
}

/**
 *
 */
PHP_METHOD(Phalcon_Binary_Writer, writeFloat){

	zval *num, length = {}, format = {}, result = {};

	phalcon_fetch_params(0, 1, 0, &num);

	ZVAL_LONG(&length, sizeof(float));

	ZVAL_STRING(&format, "f");
	PHALCON_CALL_FUNCTIONW(&result, "pack", &format, num);

	PHALCON_CALL_METHODW(NULL, getThis(), "write", &result, &length);
	RETURN_THISW();
}

/**
 *
 */
PHP_METHOD(Phalcon_Binary_Writer, writeDouble){

	zval *num, length = {}, format = {}, result = {};

	phalcon_fetch_params(0, 1, 0, &num);

	ZVAL_LONG(&length, sizeof(double));

	ZVAL_STRING(&format, "d");
	PHALCON_CALL_FUNCTIONW(&result, "pack", &format, num);

	PHALCON_CALL_METHODW(NULL, getThis(), "write", &result, &length);
	RETURN_THISW();
}

/**
 *
 */
PHP_METHOD(Phalcon_Binary_Writer, writeString){

	zval *str, *length = NULL, *exact = NULL, len = {}, format = {}, result = {};

	phalcon_fetch_params(0, 1, 2, &str, &length, &exact);

	if (length && Z_TYPE_P(length) != IS_NULL) {
		if (exact && zend_is_true(exact)) {
			PHALCON_CONCAT_SV(&format, "a", length);
		} else {
			PHALCON_CONCAT_SV(&format, "Z", length);
		}
	} else {
		if (exact && zend_is_true(exact)) {
			ZVAL_STRING(&format, "a*");
		} else {
			ZVAL_STRING(&format, "Z*");
		}
	}
	PHALCON_CALL_FUNCTIONW(&result, "pack", &format, str);

	ZVAL_LONG(&len, Z_STRLEN(result));

	PHALCON_CALL_METHODW(NULL, getThis(), "write", &result, &len);
	RETURN_THISW();
}

/**
 *
 */
PHP_METHOD(Phalcon_Binary_Writer, writeHexString){

	zval *str, *length = NULL, *low_nibble = NULL, len = {}, format = {}, result = {};

	phalcon_fetch_params(0, 1, 2, &str, &length, &low_nibble);

	if (length && Z_TYPE_P(length) != IS_NULL) {
		if (low_nibble && zend_is_true(low_nibble)) {
			PHALCON_CONCAT_SV(&format, "h", length);
		} else {
			PHALCON_CONCAT_SV(&format, "H", length);
		}
	} else {
		if (low_nibble && zend_is_true(low_nibble)) {
			ZVAL_STRING(&format, "h*");
		} else {
			ZVAL_STRING(&format, "H*");
		}
	}
	PHALCON_CALL_FUNCTIONW(&result, "pack", &format, str);

	ZVAL_LONG(&len, Z_STRLEN(result));

	PHALCON_CALL_METHODW(NULL, getThis(), "write", &result, &len);
	RETURN_THISW();
}
