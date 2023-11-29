static void test_calc_uptime_hour_minute(void **state) {
    char *uptime_str = NULL;

    UNUSED(state);

    /* Make sure the passed 'in' argument is correct */
    expect_string(__wrap_uptime, uptime_path, "/proc/uptime");

    /* We tell the uptime function what values it should return */
    will_return(__wrap_uptime, 3699.16);
    will_return(__wrap_uptime, 4069.23);

    /* We call the function like we would do it normally */
    uptime_str = calc_uptime();

    /* Now lets check if the result is what we expect it to be */
    assert_non_null(uptime_str);
    assert_string_equal(uptime_str, "up 1 hour, 1 minute");

    free(uptime_str);
}