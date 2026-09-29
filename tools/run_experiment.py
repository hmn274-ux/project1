"""C 실험을 한 번 실행해 원본 CSV, 환경 정보, 안정성 예제를 저장한다."""
import csv
import hashlib
import io
import json
import platform
import subprocess
from datetime import datetime, timezone
from pathlib import Path


def main():
    root = Path(__file__).resolve().parents[1]
    executable = root / "src" / "main.out"

    def run(*args):
        return subprocess.run(
            [str(executable), *args], cwd=root, check=True,
            capture_output=True, text=True
        ).stdout

    csv_text = run("--csv")
    rows = list(csv.DictReader(io.StringIO(csv_text)))
    if len(rows) != 48 or any(
        row["sorted"] != "yes" or row["permutation_preserved"] != "yes"
        for row in rows
    ):
        raise RuntimeError("Expected 48 valid experiment rows")
    environment = {
        "measured_at_utc": datetime.now(timezone.utc).isoformat(),
        "system": platform.system(),
        "release": platform.release(),
        "machine": platform.machine(),
        "processor": platform.processor(),
        "cpu_model": "not available",
        "python": platform.python_version(),
        "program_environment": run("--environment").splitlines(),
        "source_sha256": {
            str(path.relative_to(root)): hashlib.sha256(path.read_bytes()).hexdigest()
            for path in sorted((root / "src").iterdir())
            if path.suffix in {".c", ".h"}
        },
        "note": "These files describe the machine running this command; not necessarily Codespaces.",
    }
    cpuinfo = Path("/proc/cpuinfo")
    if cpuinfo.exists():
        for line in cpuinfo.read_text().splitlines():
            if line.startswith("model name"):
                environment["cpu_model"] = line.split(":", 1)[1].strip()
                break
    demo_text = run("--demo")
    output = root / "report"
    output.mkdir(exist_ok=True)
    (output / "results.csv").write_text(csv_text, encoding="utf-8")
    (output / "environment.json").write_text(
        json.dumps(environment, ensure_ascii=False, indent=2) + "\n", encoding="utf-8"
    )
    (output / "stability_demo.txt").write_text(demo_text, encoding="utf-8")
    print("Saved 48 rows to report/results.csv, plus environment.json and stability_demo.txt")


if __name__ == "__main__":
    main()
