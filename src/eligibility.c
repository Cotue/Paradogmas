#include "eligibility.h"
#include <stddef.h>

void evaluate_course_eligibility(Course *course, const StudentHistory *history){
    if(course == NULL || history == NULL){
        return;
    
    }

    if(code_list_contains(&history->approved_courses,course->code)){
        course->can_enroll=false;
        return;
    } //inicialmente lo ponemos en true
    for (int i=0; i< (int)course->requirements.count;i++){
        const char *req_code = course->requirements.items[i];

        if(code_list_contains(&history->approved_courses,req_code)==0){
            course->can_enroll=false;
            break;
        }
    }
    code_list_free(&course->pending_corequisites);
    code_list_init(&course->pending_corequisites);

    for(int i=0;i< (int)course->corequisites.count;i++){
        const char *coreq_code= course->corequisites.items[i];
        if(code_list_contains(&history->approved_courses,coreq_code)==0){
            code_list_add(&course->pending_corequisites,coreq_code);
        }
    }
}

Status evaluate_catalog_eligibility(Catalog *catalog, const StudentHistory *history) {
    if (catalog == NULL || history == NULL) {
        return ERROR_ARGS;
    }

    // Recorrer todos los cursos del catálogo de forma lineal
    for (int i = 0; i < (int)catalog->course_count; i++) {
        evaluate_course_eligibility(&catalog->courses[i], history);
    }

    return SUCCESS;
}