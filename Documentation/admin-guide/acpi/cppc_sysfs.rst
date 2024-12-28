.. SPDX-License-Identifier: GPL-2.0

==================================================
공동 프로세서 성능 제어 (Collaborative Processor Performance Control, CPPC)
==================================================

.. _cppc_sysfs:

CPPC
====

CPPC는 ACPI 스펙에 정의되어 있으며, 논리 프로세서의 성능을 연속적이고 추상화된(performance scale) 방식으로 운영체제(OS)가
관리할 수 있도록 하는 메커니즘입니다. CPPC는 추상화된 성능 스케일을 설명하기 위해, 그리고 원하는 성능 레벨을 요청하거나 CPU 단위로
실제 제공된(delivered) 성능을 측정하기 위해 사용할 수 있는 일련의 레지스터를 노출합니다.

자세한 사항은 ACPI 스펙 (UEFI 웹 사이트)를 참고하시기 바랍니다.

http://uefi.org/specifications

이 중 일부 CPPC 레지스터는 `sysfs`경로를 통해 아래와 같이 제공됩니다::

  /sys/devices/system/cpu/cpuX/acpi_cppc/

각 CPU X에 대해해::

  $ ls -lR  /sys/devices/system/cpu/cpu0/acpi_cppc/
  /sys/devices/system/cpu/cpu0/acpi_cppc/:
  total 0
  -r--r--r-- 1 root root 65536 Mar  5 19:38 feedback_ctrs
  -r--r--r-- 1 root root 65536 Mar  5 19:38 highest_perf
  -r--r--r-- 1 root root 65536 Mar  5 19:38 lowest_freq
  -r--r--r-- 1 root root 65536 Mar  5 19:38 lowest_nonlinear_perf
  -r--r--r-- 1 root root 65536 Mar  5 19:38 lowest_perf
  -r--r--r-- 1 root root 65536 Mar  5 19:38 nominal_freq
  -r--r--r-- 1 root root 65536 Mar  5 19:38 nominal_perf
  -r--r--r-- 1 root root 65536 Mar  5 19:38 reference_perf
  -r--r--r-- 1 root root 65536 Mar  5 19:38 wraparound_time

* highest_perf : 이 프로세서가 낼 수 있는 가장 높은 성능(추상화된 스케일).
* nominal_perf : 이 프로세서가 지속적으로 낼 수 있는 최고 성능(추상화된 스케일).
* lowest_nonlinear_perf : 비선형적인(nonlinear) 전력 절감이 적용되는 가장 낮은 성능(추상화된 스케일).
* lowest_perf : 이 프로세서가 낼 수 있는 가장 낮은 성능(추상화된 스케일).

* lowest_freq : `lowest_perf`와 대응하는 CPU 주파수(MHz 단위).
* nominal_freq : `nominal_perf`와 대응하는 CPU 주파수수 (MHz 단위).
  위 주파수 값들은 "추상화된 스케일" 대신 주파수로 성능을 표시할 때만 참조해야 합니다.
  실제 기능 결정이나 내부 로직에 사용해서는 안 됩니다.

* feedback_ctrs : 참조(reference) 성능 카운터와 실제 전달된(delivered) 성능 카운터가 함께 들어 있습니다.
  참조 카운터는 프로세서의 "기준(reference) 성능"에 비례해 증가합니다.
  전달된(delivered) 카운터는 프로세서가 실제로 제공한 성능에 비례해 증가합니다.
* wraparound_time: 이 피드백 카운터가 오버플로(래핑)될 때까지 걸리는 최소 시간(초 단위).
* reference_perf : 참조 성능 카운터가 누적되는 기준(performance level, 추상화된 스케일).


평균 전달 성능(Average Delivered Performance) 계산산
=======================================

아래는 시간을 T1, T2로 나누어 두 시점에서 피드백 카운터를 읽은 뒤, 그 사이에 제공된 평균 성능을 계산하는 과정을 설명합니다.

  T1: 시간 T1에 `feedback_ctrs`(피드백 카운터)를 읽어 `fbc_t1`으로 저장.
      이후 일정 시간 대기하거나 워크로드를 수행.

  T2: 시간 T2에 다시 `feedback_ctrs`를 읽어 `fbc_t2`로 저장.

이 때, 예를 들어 다음처럼 계산할 수 있습니다(의사코드)::

  delivered_counter_delta = fbc_t2[del] - fbc_t1[del] # 전달된(delivered) 카운터 차이
  reference_counter_delta = fbc_t2[ref] - fbc_t1[ref] # 참조(reference) 카운터 차이이

  delivered_perf = (reference_perf x delivered_counter_delta) / reference_counter_delta

* delivered_perf: CPU가 실제로 제공한 성능(추상화된 스케일)
  위 식에서 `reference_perf`는 `/sys/devices/system/cpu/cpuX/acpi_cppc/reference_perf`에 정의된 값입니다.

결과적으로 이렇게 얻어진 `delivered_perf`를 통해 측정 구간(T1 ~ T2) 동안 CPU가 어느 정도 성능을 실제로 낼 수 있었는지(추상화된 스케일) 가늠할 수 있습니다.
