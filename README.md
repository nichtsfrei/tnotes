# tnotes - Todays Notes

Is small program for parsing `*.md` files in given Dir for the rules:

- ON 2021-01-01
- FROM 2022-02-02
- UNTIL 2023-03-03
- ON_WEEKDAY
- ON_WEEKEND

to detect relevant markdown files for the day and print them as a link:

```
> ./tnotes /tmp/example/
file:///tmp/example//3/3.md
```

Further plans:
- Add flake for easy integration
- Add RULE working day as short form for ON_WEEKDAY and information from until the vacatio lasts to ignore work related markdown files.
