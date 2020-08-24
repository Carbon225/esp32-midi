#ifndef COMPONENTS_ERROR_CHECK_RETURN_INCLUDE_ERROR_CHECK_RETURN_H_
#define COMPONENTS_ERROR_CHECK_RETURN_INCLUDE_ERROR_CHECK_RETURN_H_

#define ESP_ERROR_CHECK_RETURN(fn) ({                           \
	esp_err_t __ret = ESP_ERROR_CHECK_WITHOUT_ABORT((fn));      \
	if (__ret != ESP_OK) {                                      \
		return __ret;                                           \
	}                                                           \
	__ret;														\
})

#endif /* COMPONENTS_ERROR_CHECK_RETURN_INCLUDE_ERROR_CHECK_RETURN_H_ */
