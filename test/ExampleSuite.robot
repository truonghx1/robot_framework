*** Settings ***
Resource   ../resources/common.robot

*** Variables ***
${nameVal}    myValue    

*** Test Cases ***
Test case name 1
    [Tags]    Smoke
    Common KeyWork 1
    Common KeyWork 2
    Hello Example Library

Test case name 2
    [Tags]    Regression
    Common KeyWork 1
    Common KeyWork 2
    ${result}=    Add Two Numbers    5    10
    Should Be Equal As Numbers    ${result}    15



