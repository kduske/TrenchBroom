# Generate help documents
SET(DOC_DIR "${CMAKE_SOURCE_DIR}/app/resources/documentation")
SET(DOC_MANUAL_SOURCE_DIR "${DOC_DIR}/manual")
SET(DOC_MANUAL_TARGET_DIR "${CMAKE_CURRENT_BINARY_DIR}/gen-manual")

SET(DOC_MANUAL_SOURCE_IMAGES_DIR "${DOC_MANUAL_SOURCE_DIR}/images")
SET(DOC_MANUAL_TARGET_IMAGES_DIR "${DOC_MANUAL_TARGET_DIR}/images")

SET(PANDOC_TEMPLATE_PATH "${DOC_MANUAL_SOURCE_DIR}/template.html")
SET(PANDOC_INPUT_PATH    "${DOC_MANUAL_SOURCE_DIR}/index.md")
SET(PANDOC_OUTPUT_PATH   "${DOC_MANUAL_TARGET_DIR}/index.html.tmp")
SET(INDEX_OUTPUT_PATH    "${DOC_MANUAL_TARGET_DIR}/index.html")

FIX_WIN32_PATH(PANDOC_TEMPLATE_PATH)
FIX_WIN32_PATH(PANDOC_INPUT_PATH)
FIX_WIN32_PATH(PANDOC_OUTPUT_PATH)

ADD_CUSTOM_COMMAND(OUTPUT "${DOC_MANUAL_TARGET_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${DOC_MANUAL_TARGET_DIR}"
)

ADD_CUSTOM_COMMAND(OUTPUT "${DOC_MANUAL_TARGET_IMAGES_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${DOC_MANUAL_TARGET_IMAGES_DIR}"
)

ADD_CUSTOM_COMMAND(OUTPUT "${INDEX_OUTPUT_PATH}"
    COMMAND pandoc --standalone --toc --toc-depth=2 --template "${PANDOC_TEMPLATE_PATH}" --from=markdown --to=html5 -o "${PANDOC_OUTPUT_PATH}" "${PANDOC_INPUT_PATH}"
    COMMAND ${CMAKE_COMMAND} -DINPUT="${PANDOC_OUTPUT_PATH}" -DOUTPUT="${INDEX_OUTPUT_PATH}" -P "${CMAKE_SOURCE_DIR}/cmake/TransformKeyboardShortcuts.cmake"
    COMMAND ${CMAKE_COMMAND} -E remove "${PANDOC_OUTPUT_PATH}"
    DEPENDS "${DOC_MANUAL_TARGET_DIR}" "${PANDOC_TEMPLATE_PATH}" "${PANDOC_INPUT_PATH}" "${CMAKE_SOURCE_DIR}/cmake/TransformKeyboardShortcuts.cmake"
)

SET(DOC_MANUAL_TARGET_FILES "${INDEX_OUTPUT_PATH}")

FILE(GLOB_RECURSE DOC_MANUAL_SOURCE_FILES
    "${DOC_MANUAL_SOURCE_DIR}/*.css"
    "${DOC_MANUAL_SOURCE_DIR}/*.js"
)

FOREACH(MANUAL_FILE ${DOC_MANUAL_SOURCE_FILES})
	GET_FILENAME_COMPONENT(MANUAL_FILE_NAME "${MANUAL_FILE}" NAME)
	SET(DOC_MANUAL_TARGET_FILES 
		${DOC_MANUAL_TARGET_FILES}
		"${DOC_MANUAL_TARGET_DIR}/${MANUAL_FILE_NAME}"
	)

	ADD_CUSTOM_COMMAND(OUTPUT "${DOC_MANUAL_TARGET_DIR}/${MANUAL_FILE_NAME}"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${MANUAL_FILE}" "${DOC_MANUAL_TARGET_DIR}"
		DEPENDS "${DOC_MANUAL_TARGET_DIR}" "${MANUAL_FILE}"
	)
ENDFOREACH(MANUAL_FILE)

# Copy the images as well

FILE(GLOB_RECURSE DOC_MANUAL_SOURCE_IMAGE_FILES
    "${DOC_MANUAL_SOURCE_IMAGES_DIR}/*.png"
    "${DOC_MANUAL_SOURCE_IMAGES_DIR}/*.gif"
)

FOREACH(IMAGE_FILE ${DOC_MANUAL_SOURCE_IMAGE_FILES})
	GET_FILENAME_COMPONENT(IMAGE_FILE_NAME "${IMAGE_FILE}" NAME)
	SET(DOC_MANUAL_TARGET_IMAGE_FILES
		${DOC_MANUAL_TARGET_IMAGE_FILES}
		"${DOC_MANUAL_TARGET_IMAGES_DIR}/${IMAGE_FILE_NAME}"
	)

	ADD_CUSTOM_COMMAND(OUTPUT "${DOC_MANUAL_TARGET_IMAGES_DIR}/${IMAGE_FILE_NAME}"
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${IMAGE_FILE}" "${DOC_MANUAL_TARGET_IMAGES_DIR}"
		DEPENDS "${DOC_MANUAL_TARGET_IMAGES_DIR}" "${IMAGE_FILE}"
	)
ENDFOREACH(IMAGE_FILE)

ADD_CUSTOM_TARGET(GenerateManual DEPENDS ${DOC_MANUAL_TARGET_FILES} ${DOC_MANUAL_TARGET_IMAGE_FILES})
