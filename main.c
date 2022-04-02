#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {false, true} boolean;

typedef struct Intervals
{
    int start_hour;
    int start_minute;
    boolean set_start;
    int end_hour;
    int end_minute;
    boolean set_end;
} Intervals;

int digits(int n){
    if(n == 0)
        return 0;
    else
        return 1 + digits(n/10);
}

int min(int x, int y){
    if(x < y)
        return x;
    return y;
}

int max(int x, int y){
    if(x > y)
        return x;
    return y;
}

/** parse range_limit string in 4 parts and return them **/
int *get_limits(char *range_limit){
    int *limits = (int *)calloc(4, sizeof(int)); // 4 -> start_hour, start_minute, end_hour, end_minute
    char *token = strtok(range_limit, ",");
    int start_hour = atoi(token + 2);
    int digits_start_hour = digits(start_hour);
    int start_minute = (digits_start_hour == 1) ? atoi(token + 4) : atoi(token + 5);

    token = strtok(NULL, ",");
    int end_hour = atoi(token + 1);
    int digits_end_hour = digits(end_hour);
    int end_minute = (digits_end_hour == 1) ? atoi(token + 3) : atoi(token + 4);

    limits[0] = start_hour;
    limits[1] = start_minute;
    limits[2] = end_hour;
    limits[3] = end_minute;
    return limits;
}


/** set an array with values of 0's or 1's, based on whether the 2 people are free (they can meet) - '0' or at least
    one of them is busy - '1' **/
void set_minute_array(int *arr, Intervals *schedule_one, Intervals *schedule_two, int size_schedule_one, int size_schedule_two)
{
    int start_time_sch1 = 0;
    int end_time_sch1   = 0;
    int start_time_sch2 = 0;
    int end_time_sch2   = 0;

    for(int i = 0; i < size_schedule_one; i++){
        start_time_sch1 = schedule_one[i].start_hour * 60 + schedule_one[i].start_minute;
        end_time_sch1   = schedule_one[i].end_hour * 60 + schedule_one[i].end_minute;
        for(int j = start_time_sch1; j < end_time_sch1; j++)
            arr[j] = 1;
    }

    for(int i = 0; i < size_schedule_two; i++){
        start_time_sch2 = schedule_two[i].start_hour * 60 + schedule_two[i].start_minute;
        end_time_sch2   = schedule_two[i].end_hour * 60 + schedule_two[i].end_minute;
        for(int j = start_time_sch2; j < end_time_sch2; j++)
            arr[j] = 1;
    }
}

void print_meeting_intervals(int *arr, int start_time, int end_time, int meeting_duration){
    int i, j;
    for(i = start_time; i <= end_time - meeting_duration; i++){
        int count_minutes = 1;
        boolean print_interval = false;
        for(j = i + 1; j <= end_time; j++){
            if(arr[i] == 0 && arr[j] == 0)
                count_minutes++;
            if(arr[i] != arr[j]){
                print_interval = true;
                break;
            }
        }
        if(count_minutes >= meeting_duration && print_interval == true)
            printf("[%d, %d]", i, j);
    }
}

int main()
{
    char *calendar_one = (char *)malloc(1000 * sizeof(char));
    char *calendar_two = (char *)malloc(1000 * sizeof(char));
    char *range_limit_one = (char *)malloc(100 * sizeof(char));
    char *range_limit_two = (char *)malloc(100 * sizeof(char));

    int given_hours = 0; /** given_hours % 2 == 0 => start_hour/minute; given_hours %2 == 1 => end_hour/minute **/

    Intervals intervals_calendar_one[30];
    Intervals intervals_calendar_two[30];
    int index1 = 0;
    int index2 = 0;

    /** 1440 because there are 24 hours and 60 minutes / hour; 24*60 = 1440. Each index represents one minute in a day.
        Eg: minute_array[0] = 00:00(hh:mm)
            minute_array[61] = 1:01(hh:mm)
    **/
    int minute_array[1440] = {0}; /** minute_array[i] = '0' -> free time; minute_array[i] = '1' -> scheduled / busy **/
    int meeting_duration = 0;

    for(int i= 0; i < 30; i ++)
    {
        (intervals_calendar_one + i) -> set_start = (intervals_calendar_one + i) -> set_end = false;
        (intervals_calendar_two + i) -> set_start = (intervals_calendar_two + i) -> set_end = false;
    }

    printf("Booked calendar 1: ");
    if (fgets(calendar_one, 1000, stdin) != NULL)
    {
        char *token = strtok(calendar_one, ",");
        while (token != NULL)
        {
            printf("%s\n", token);
            if(given_hours % 2 == 0){
                int hour = atoi(token + 3);
                int digits_hour = digits(hour); // possible values: 1 or 2
                int minute = (digits_hour == 1) ? atoi(token + 5) : atoi(token + 6);
                int digits_minute = digits(minute); // possible values: 1 or 2. "05" -> 5; "15" -> 15
                intervals_calendar_one[index1].start_hour = hour;
                intervals_calendar_one[index1].start_minute = minute;
                intervals_calendar_one[index1].set_start = true;
            }
            else if(given_hours % 2 == 1){
                int hour = atoi(token + 1);
                int digits_hour = digits(hour);
                int minute = (digits_hour == 1) ? atoi(token + 3) : atoi(token + 4);
                int digits_minute = digits(minute);
                intervals_calendar_one[index1].end_hour = hour;
                intervals_calendar_one[index1].end_minute = minute;
                intervals_calendar_one[index1].set_end = true;
            }
            if(intervals_calendar_one[index1].set_start == true && intervals_calendar_one[index1].set_end == true)
                index1++;
            token = strtok(NULL, ",");
            given_hours++;
        }

        for(int i=0; i<index1; i++){
            printf("(%d : %d) -> (%d : %d)\n",
                   intervals_calendar_one[i].start_hour, intervals_calendar_one[i].start_minute,
                   intervals_calendar_one[i].end_hour, intervals_calendar_one[i].end_minute);
        }
    }

    printf("\nRange limit calendar 1: ");
    fgets(range_limit_one, 100, stdin);
    int *limits = get_limits(range_limit_one);

    int start_hour_calendar1   = limits[0];
    int start_minute_calendar1 = limits[1];
    int end_hour_calendar1     = limits[2];
    int end_minute_calendar1   = limits[3];

    int start_calendar1_to_array_index = start_hour_calendar1 * 60 + start_minute_calendar1;
    int end_calendar1_to_array_index   = end_hour_calendar1   * 60 + end_minute_calendar1;

    printf("\n%d %d\n", start_calendar1_to_array_index, end_calendar1_to_array_index);



    // Calendar 2:
    printf("Booked calendar 2: ");
    if (fgets(calendar_two, 1000, stdin) != NULL)
    {
        char *token = strtok(calendar_two, ",");
        while (token != NULL)
        {
            printf("%s\n", token);
            if(given_hours % 2 == 0){
                int hour = atoi(token + 3);
                int digits_hour = digits(hour); // possible values: 1 or 2
                int minute = (digits_hour == 1) ? atoi(token + 5) : atoi(token + 6);
                int digits_minute = digits(minute); // possible values: 1 or 2. "05" -> 5; "15" -> 15
                intervals_calendar_two[index2].start_hour = hour;
                intervals_calendar_two[index2].start_minute = minute;
                intervals_calendar_two[index2].set_start = true;
            }
            else if(given_hours % 2 == 1){
                int hour = atoi(token + 1);
                int digits_hour = digits(hour);
                int minute = (digits_hour == 1) ? atoi(token + 3) : atoi(token + 4);
                int digits_minute = digits(minute);
                intervals_calendar_two[index2].end_hour = hour;
                intervals_calendar_two[index2].end_minute = minute;
                intervals_calendar_two[index2].set_end = true;
            }
            if(intervals_calendar_two[index2].set_start == true && intervals_calendar_two[index2].set_end == true)
                index2++;
            token = strtok(NULL, ",");
            given_hours++;
        }

        for(int i=0; i<index2; i++){
            printf("(%d : %d) -> (%d : %d)\n",
                   intervals_calendar_two[i].start_hour, intervals_calendar_two[i].start_minute,
                   intervals_calendar_two[i].end_hour, intervals_calendar_two[i].end_minute);
        }
    }

    printf("\nRange limit calendar 2: ");
    fgets(range_limit_two, 100, stdin);
    limits = get_limits(range_limit_two);

    int start_hour_calendar2   = limits[0];
    int start_minute_calendar2 = limits[1];
    int end_hour_calendar2     = limits[2];
    int end_minute_calendar2   = limits[3];

    int start_calendar2_to_array_index = start_hour_calendar2 * 60 + start_minute_calendar2;
    int end_calendar2_to_array_index   = end_hour_calendar2   * 60 + end_minute_calendar2;

    printf("\n%d %d\n", start_calendar2_to_array_index, end_calendar2_to_array_index);


    /** Meeting duration: **/
    printf("\nMeeting duration: ");
    scanf("%d", &meeting_duration);

    /** Get the start_time and the end_time **/
    int start_time = max(start_calendar1_to_array_index, start_calendar2_to_array_index);
    int end_time   = min(end_calendar1_to_array_index, end_calendar2_to_array_index);

    /** Get the minute_array **/
    set_minute_array(minute_array, &intervals_calendar_one, &intervals_calendar_two, index1, index2);
    for(int i=0; i < 1440; i ++){
        printf("%d ", minute_array[i]);
    }

    printf("\n\n");
    print_meeting_intervals(minute_array, start_time, end_time, meeting_duration);

    return 0;
}


