*** Settings ***
Library    ../libs/ExampleLibrary.py

*** Variables ***
${myVal}    123

*** Keywords ***
Common KeyWork 1
    [Documentation]    Set value for ${myVal} to 1234
    [Tags]    Keywork1
    Set Suite Variable    ${myVal}    1234
    Log    message="Update value myVal ${myVal}"
Common KeyWork 2
    [Documentation]    Set value for ${myVal} to 321
    [Tags]    Keywork2
    Set Suite Variable    ${myVal}    321
    Log    message="Update value myVal ${myVal}"

Add Two Numbers
    [Arguments]    ${number1}    ${number2}
    ${sum}=    Evaluate    ${number1} + ${number2}
    [Return]    ${sum}