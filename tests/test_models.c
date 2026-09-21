#include <assert.h>
#include <stddef.h>

#include "models.h"

int main(void)
{
    Catalog catalog;
    StudentHistory history;
    Course course;
    Group group;
    Schedule schedule;
    CodeList list;

    catalog_init(&catalog);
    student_history_init(&history);
    course_init(&course);
    group_init(&group);
    schedule_init(&schedule);
    code_list_init(&list);

    assert(catalog.courses == NULL);
    assert(catalog.course_count == 0);

    assert(history.approved_courses.count == 0);

    assert(course.code == NULL);
    assert(course.groups == NULL);
    assert(course.group_count == 0);

    assert(group.schedules == NULL);
    assert(group.schedule_count == 0);
    assert(group.has_conflict == false);

    assert(schedule.day == DAY_INVALID);
    assert(schedule.start_minutes == -1);
    assert(schedule.end_minutes == -1);

    assert(list.items == NULL);
    assert(list.count == 0);

    code_list_free(&list);
    schedule_free(&schedule);
    group_free(&group);
    course_free(&course);
    student_history_free(&history);
    catalog_free(&catalog);

    return 0;
}