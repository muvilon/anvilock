#!/bin/bash

# Function to compare versions
version_greater_than() {
    # Compare two version strings
    # Returns 0 (true) if $1 is greater than $2, 1 (false) otherwise
    local v1="$1"
    local v2="$2"
    [ "$(printf '%s\n' "$v1" "$v2" | sort -V | head -n1)" = "$v2" ]
}

# Function to generate the CHANGELOG
generate_changelog() {
    # Read the current version from the VERSION file
    current_version=$(<../VERSION)

    echo "Current version: $current_version"

    # Get user input for the new version
    while true; do
        version=$(gum input --placeholder "Enter new version (greater than $current_version)")
        
        # Validate that the new version is greater than the current version
        if version_greater_than "$version" "$current_version"; then
            break
        else
            gum style --foreground "red" "Invalid version! Please enter a version greater than $current_version."
        fi
    done

    # Validate type (must be ALPHA)
    type="ALPHA"
    gum style --foreground "green" "Type is set to: **$type**"

    # Validate commit type
    commit_type=$(gum choose --header "Select commit type:" "MEDIUM" "CHORE" "MAJOR" "VULN" "MINOR" "DOCS")

    echo "Enter summary of changes (type 'done' when finished):"
    changes=()
    
    while true; do
        change=$(gum input --placeholder "Enter change (type 'done' when finished)")
        if [ "$change" == "done" ]; then
            break
        fi
        changes+=("* $change")
    done

    # Get footer information
    author=$(gum input --placeholder "Enter author's name")
    date=$(gum input --placeholder "Enter date (e.g., 18/10/2024)")

    # Create CHANGELOG.md content
    changelog_content="# CHANGELOG FILE

This is a CHANGELOG FILE generated for [anvilock](https://github.com/muvilon/anvilock)

## Version: $version

---

## Type: **$type**

---

## Commit type: **$commit_type**

### Summary of Changes

$(printf "%s\n" "${changes[@]}")

## Footer

CHANGELOG FILE authored by $author on $date."

    # Create the CHANGELOG directory if it doesn't exist
    mkdir -p CHANGELOG

    # Write to CHANGELOG/v$version-alpha.md
    echo "$changelog_content" > "CHANGELOG/v$version-alpha.md"

    gum style --foreground "green" "CHANGELOG/v$version-alpha.md has been generated successfully!"
}

# Execute the function
generate_changelog
